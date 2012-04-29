#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <libipctl/ipctl.h>
#include <libipctl/ipctl-nl.h>
#include <unistd.h>

int
ipctl_lookup_genl_family(struct nl_sock *socket)
{
  return genl_ctrl_resolve(socket, IPCTL_GENL_NAME);
}


int
ipctl_set_proxy_arp(struct nl_sock *socket, int family, int ifIndex, int on)
{
  int rc;

  struct nl_msg *msg;
  msg = nlmsg_alloc();

  genlmsg_put(msg, NL_AUTO_PID, NL_AUTO_SEQ, family, 0, NLM_F_ECHO,
	      IPCTL_CMD_SET, IPCTL_GENL_VERSION);
  nla_put_u32(msg, IPCTL_ATTR_PROPERTY, IPCTL_PROPERTY_PROXYARP);
  nla_put_u32(msg, IPCTL_ATTR_IFINDEX, ifIndex);
  nla_put_u8(msg, IPCTL_ATTR_VALUE, on);
  
  // Send message over netlink socket
  rc = nl_send_auto_complete(socket, msg);
  
  // Free message
  nlmsg_free(msg);
  
  if (rc < 0)
    return rc;

  return 0;
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
  genlmsg_parse(nlh, 0, attrs, IPCTL_ATTR_MAX, ipctl_genl_policy);

  int property;
  int ifIndex;
  int value;
    
  if (attrs[IPCTL_ATTR_PROPERTY]) {
    property = nla_get_u32(attrs[IPCTL_ATTR_PROPERTY]);
  }

  if (attrs[IPCTL_ATTR_IFINDEX]) {
    ifIndex = nla_get_u32(attrs[IPCTL_ATTR_IFINDEX]);
  }

  if (attrs[IPCTL_ATTR_VALUE]) {
    value = nla_get_u8(attrs[IPCTL_ATTR_VALUE]);
  }

  if (arg)
    *((int*)arg) = value;

  return 0;
}


int
ipctl_get_proxy_arp(struct nl_sock *socket, int family, int ifIndex, int *response)
{
  struct nl_msg *msg;
  int rc;
  msg = nlmsg_alloc();

  genlmsg_put(msg, NL_AUTO_PID, NL_AUTO_SEQ, family, 0, NLM_F_REQUEST,
	      IPCTL_CMD_GET, IPCTL_GENL_VERSION);
  nla_put_u32(msg, IPCTL_ATTR_PROPERTY, IPCTL_PROPERTY_PROXYARP);
  nla_put_u32(msg, IPCTL_ATTR_IFINDEX, ifIndex);
  nla_put_u8(msg, IPCTL_ATTR_VALUE, 0);
  
  // Send message over netlink socket
  rc = nl_send_auto_complete(socket, msg);

  // Free message
  nlmsg_free(msg);

  if (rc < 0)
    return rc;

  // Prepare callback to receive response.
  rc = nl_socket_modify_cb(socket, NL_CB_VALID, NL_CB_CUSTOM,
		                       parse_get_response, response);
  if (rc < 0)
    return rc;
 
  // Wait for the answer and receive it
  int recv_code = nl_recvmsgs_default(socket);

  // Remove callback.
  rc = nl_socket_modify_cb(socket, NL_CB_VALID, NL_CB_CUSTOM, NULL, NULL);

  if (recv_code)
     return recv_code;

  /* Wait for the ACK to be received, otherwise the buffers may
   * fill up if the client sends out requests too fast. */
  rc = nl_wait_for_ack(socket);
  if (rc)
    return rc;

  return 0;
}


