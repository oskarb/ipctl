#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <libipctl/ipctl.h>
#include <libipctl/ipctl-nl.h>

int
ipctl_set_proxy_arp(struct nl_sock *socket, int family, int ifIndex, int on)
{
  struct nl_msg *msg;
  msg = nlmsg_alloc();

  genlmsg_put(msg, NL_AUTO_PID, NL_AUTO_SEQ, family, 0, NLM_F_ECHO,
	      IPCTL_CMD_SET, IPCTL_GENL_VERSION);
  nla_put_u32(msg, IPCTL_ATTR_PROPERTY, IPCTL_PROPERTY_PROXYARP);
  nla_put_u32(msg, IPCTL_ATTR_IFINDEX, ifIndex);
  nla_put_u8(msg, IPCTL_ATTR_VALUE, on);
  
  // Send message over netlink socket
  nl_send_auto_complete(socket, msg);
  
  // Free message
  nlmsg_free(msg);
  
  // Prepare socket to receive the answer by specifying the callback
  // function to be called for valid messages.
  //nl_socket_modify_cb(socket, NL_CB_VALID, NL_CB_CUSTOM, parse_cb, NULL);
  
  // Wait for the answer and receive it
  //nl_recvmsgs_default(socket);
}


static int parse_get_response(struct nl_msg *msg, void *arg)
{
  struct nlmsghdr *nlh = nlmsg_hdr(msg);
  struct nlattr *attrs[IPCTL_ATTR_MAX+1];
  
  // Validate message and parse attributes
  /*genlmsg_parse(nlh, 0, attrs, IPCTL_ATTR_MAX, policy);
    
    if (attrs[ATTR_FOO]) {
    uint32_t value = nla_get_u32(attrs[ATTR_FOO]);
    ...
    }
  */
  return 0;
}


int
ipctl_get_proxy_arp(struct nl_sock *socket, int family, int ifIndex, int *response)
{
  struct nl_msg *msg;
  msg = nlmsg_alloc();

  genlmsg_put(msg, NL_AUTO_PID, NL_AUTO_SEQ, family, 0, NLM_F_ECHO,
	      IPCTL_CMD_GET, IPCTL_GENL_VERSION);
  nla_put_u32(msg, IPCTL_ATTR_PROPERTY, IPCTL_PROPERTY_PROXYARP);
  nla_put_u32(msg, IPCTL_ATTR_IFINDEX, ifIndex);
  
  // Send message over netlink socket
  nl_send_auto_complete(socket, msg);
  
  // Free message
  nlmsg_free(msg);
  
  // Prepare socket to receive the answer by specifying the callback
  // function to be called for valid messages.
  nl_socket_modify_cb(socket, NL_CB_VALID, NL_CB_CUSTOM,
		      parse_get_response, response);
  
  // Wait for the answer and receive it
  nl_recvmsgs_default(socket);

  return 0;
}


