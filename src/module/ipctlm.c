#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/inetdevice.h>
#include <linux/netdevice.h>
#include <net/netlink.h>
#include <net/genetlink.h>
#include "../../include/libipctl/ipctl-nl.h"

#define MOD_AUTHOR "Oskar Berggren <oskar.berggren@gmail.com>"
#define MOD_DESC "A module to offer efficient mass control of the IP sysctl family traditionally controlled through /proc."
#define MOD_VER "0.2"


static struct genl_family ipctl_gnl_family;


static int ipctl_get_proxyarp_by_ifindex(int ifIndex, int *on)
{
	struct net *net = &init_net;
	struct net_device *dev;
	struct in_device *in_dev;

	dev = dev_get_by_index(net, ifIndex);

	if (dev)
	{
		if (__in_dev_get_rtnl(dev))
		{
			in_dev = __in_dev_get_rtnl(dev);
			*on = IN_DEV_CONF_GET(in_dev, PROXY_ARP);
		}

		dev_put(dev);  // Release reference.
	}

	return 0;
}


static int ipctl_set_proxyarp_by_ifindex(int ifIndex, int on)
{
	struct net *net = &init_net;
	struct net_device *dev;
	struct in_device *in_dev;

	dev = dev_get_by_index(net, ifIndex);

	if (dev)
	{
		if (__in_dev_get_rtnl(dev))
		{
			in_dev = __in_dev_get_rtnl(dev);
			IN_DEV_CONF_SET(in_dev, PROXY_ARP, on);
		}

		dev_put(dev);  // Release reference.
	}

	return 0;
}


static int ipctl_reply(struct sk_buff *skb, struct genl_info *info,
		       int property, int ifIndex, int value)
{
	struct sk_buff *skb_reply;
	void *msg_head;
	int rc = 0;

	pr_debug("ipctl: reply start\n");

	skb_reply = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
	if (skb_reply == NULL)
		goto out;

	msg_head = genlmsg_put(skb_reply, 0, info->snd_seq, &ipctl_gnl_family, 0, IPCTL_CMD_GET);
	if (msg_head == NULL) {
		rc = -ENOMEM;
		goto out;
	}

	rc = nla_put_u32(skb_reply, IPCTL_ATTR_PROPERTY, property);
	if (rc != 0)
		goto out;

	rc = nla_put_u32(skb_reply, IPCTL_ATTR_IFINDEX, ifIndex);
	if (rc != 0)
		goto out;

	rc = nla_put_u8(skb_reply, IPCTL_ATTR_VALUE, value);
	if (rc != 0)
		goto out;
	
	/* finalize the message */
	genlmsg_end(skb_reply, msg_head);

	rc = genlmsg_reply(skb_reply , info);
	if (rc != 0)
		goto out;

	return 0;
out:
	pr_warning("ipctl: Error occured in reply: %d\n", rc);

	return rc;
}


/* handler for SET messages via NETLINK */
int ipctl_set(struct sk_buff *skb, struct genl_info *info)
{
	/* message handling code goes here; return 0 on success, negative
	 * values on failure */

	int property = nla_get_u32(info->attrs[IPCTL_ATTR_PROPERTY]);
	int ifIndex = nla_get_u32(info->attrs[IPCTL_ATTR_IFINDEX]);
	int value = nla_get_u8(info->attrs[IPCTL_ATTR_VALUE]);

	pr_debug("ipctl: set p=%d i=%d v=%d\n", property, ifIndex, value);

	if (property == IPCTL_PROPERTY_PROXYARP)
		return ipctl_set_proxyarp_by_ifindex(ifIndex, value);

	return 0;
}


/* handler for GET messages via NETLINK */
int ipctl_get(struct sk_buff *skb, struct genl_info *info)
{
	/* message handling code goes here; return 0 on success, negative
	 * values on failure */

	int property = nla_get_u32(info->attrs[IPCTL_ATTR_PROPERTY]);
	int ifIndex = nla_get_u32(info->attrs[IPCTL_ATTR_IFINDEX]);
	int value = 0;
	int retval = 0;

	pr_debug("ipctl: get p=%d i=%d\n", property, ifIndex);

	if (property == IPCTL_PROPERTY_PROXYARP)
		retval = ipctl_get_proxyarp_by_ifindex(ifIndex, &value);

	if (retval)
		return retval;

	return ipctl_reply(skb, info, property, ifIndex, value);
}


/* NETLINK operation definition */
static struct genl_ops ipctl_gnl_ops[] = {
	{
		.cmd = IPCTL_CMD_SET,
		.flags = GENL_ADMIN_PERM,
		.policy = ipctl_genl_policy,
		.doit = ipctl_set,
		.dumpit = NULL,
	},
	{
		.cmd = IPCTL_CMD_GET,
		.flags = 0,
		.policy = ipctl_genl_policy,
		.doit = ipctl_get,
		.dumpit = NULL,
	},
};

/* family definition */
static struct genl_family ipctl_gnl_family = {
	.hdrsize = 0,
	.name = IPCTL_GENL_NAME,
	.version = IPCTL_GENL_VERSION,
	.maxattr = IPCTL_ATTR_MAX,
	.ops = ipctl_gnl_ops,
	.n_ops = ARRAY_SIZE(ipctl_gnl_ops),
};


static int __init ipctl_init(void)
{
	int rc;

	printk(KERN_INFO "ipctl: %s.\n", MOD_VER);

	rc = genl_register_family(&ipctl_gnl_family);
	if (rc)
		printk("ipctl: genl_register_family: %d.\n", rc);

	/* 
	 * A non 0 return means init_module failed; module can't be loaded. 
	 */
	return 0;
}


static void __exit ipctl_exit(void)
{
	genl_unregister_family(&ipctl_gnl_family);
}


module_init(ipctl_init);
module_exit(ipctl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(MOD_AUTHOR);
MODULE_DESCRIPTION(MOD_DESC);
MODULE_VERSION(MOD_VER);

