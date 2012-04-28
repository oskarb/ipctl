#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/init.h>	/* Needed for the init/exit macros. */
#include <linux/inetdevice.h>
#include <linux/netdevice.h>
#include <net/netlink.h>
#include <net/genetlink.h>

#define MOD_AUTHOR "Oskar Berggren <oskar.berggren@gmail.com>"
#define MOD_DESC "A module to offer efficient mass control of the IP sysctl family traditionally controlled through /proc."
#define MOD_VER "0.1"

static int ipctl_set_proxyarp_by_ifindex(int ifIndex, int on)
{
  struct net *net = &init_net;
  struct net_device *dev;
  struct in_device *in_dev;

  dev = dev_get_by_index(net, ifIndex);

  if (dev)
  {
    if (__in_dev_get_rtnl(dev)) {
      in_dev = __in_dev_get_rtnl(dev);
      IN_DEV_CONF_SET(in_dev, PROXY_ARP, on);
    }

    dev_put(dev);  // Release reference.
  }

  return 0;
}


/* attributes */
  enum {
        IPCTL_ATTR_UNSPEC,
        IPCTL_ATTR_IFINDEX,
        IPCTL_ATTR_VALUE,
        __IPCTL_ATTR_MAX,
  };
  #define IPCTL_ATTR_MAX (__IPCTL_ATTR_MAX - 1)

  /* attribute policy */
  static struct nla_policy ipctl_genl_policy[IPCTL_ATTR_MAX + 1] = {
        [IPCTL_ATTR_IFINDEX] = { .type = NLA_U32 },
        [IPCTL_ATTR_VALUE] = { .type = NLA_U8 },
  };

  /* family definition */
  static struct genl_family ipctl_gnl_family = {
        .id = GENL_ID_GENERATE,
        .hdrsize = 0,
        .name = "ipctl",
        .version = 1,
        .maxattr = IPCTL_ATTR_MAX,
 };


/* handler */
  int ipctl_set(struct sk_buff *skb, struct genl_info *info)
  {
        /* message handling code goes here; return 0 on success, negative
         * values on failure */

      int ifIndex = nla_get_u32(info->attrs[IPCTL_ATTR_IFINDEX]);
      int value = nla_get_u8(info->attrs[IPCTL_ATTR_VALUE]);
      printk(KERN_INFO "ipctl module: %d = %d.\n", ifIndex, value);

      ipctl_set_proxyarp_by_ifindex(ifIndex, value);

      return 0;
  }


  int ipctl_get(struct sk_buff *skb, struct genl_info *info)
  {
        /* message handling code goes here; return 0 on success, negative
         * values on failure */

      return 0;
  }



  /* commands */
  enum {
        IPCTL_CMD_UNSPEC,
        IPCTL_CMD_SET,
        IPCTL_CMD_GET,
        __IPCTL_CMD_MAX,
  };
  #define IPCTL_CMD_MAX (__IPCTL_CMD_MAX - 1)

  /* operation definition */
  struct genl_ops ipctl_gnl_ops_set = {
        .cmd = IPCTL_CMD_SET,
        .flags = 0,
        .policy = ipctl_genl_policy,
       	.doit = ipctl_set,
	      .dumpit = NULL,
  };
  struct genl_ops ipctl_gnl_ops_get = {
        .cmd = IPCTL_CMD_GET,
        .flags = 0,
        .policy = ipctl_genl_policy,
       	.doit = ipctl_get,
	      .dumpit = NULL,
  };








static int __init ipctl_init(void)
{
	printk(KERN_INFO "ipctl module.\n");

  genl_register_family(&ipctl_gnl_family);
  genl_register_ops(&ipctl_gnl_family, &ipctl_gnl_ops_set);
  genl_register_ops(&ipctl_gnl_family, &ipctl_gnl_ops_get);

  int ifIndex = 2;

  struct net *net = &init_net;
  struct net_device *dev;
  struct in_device *in_dev;

  dev = dev_get_by_index(net, ifIndex);

  if (dev)
  {
    if (__in_dev_get_rtnl(dev)) {
      in_dev = __in_dev_get_rtnl(dev);
      IN_DEV_CONF_SET(in_dev, PROXY_ARP, 0);
    }

    dev_put(dev);  // Release reference.
  }

	/* 
	 * A non 0 return means init_module failed; module can't be loaded. 
	 */
	return 0;
}

static void __exit ipctl_exit(void)
{
  genl_unregister_family(&ipctl_gnl_family);

	printk(KERN_INFO "ipctl module exit.\n");
}


module_init(ipctl_init);
module_exit(ipctl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(MOD_AUTHOR);
MODULE_DESCRIPTION(MOD_DESC);
MODULE_VERSION(MOD_VER);

