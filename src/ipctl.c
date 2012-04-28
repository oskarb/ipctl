#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <stdio.h>


enum {
        IPCTL_ATTR_UNSPEC,
        IPCTL_ATTR_IFINDEX,
        IPCTL_ATTR_VALUE,
        __IPCTL_ATTR_MAX,
  };
  #define IPCTL_ATTR_MAX (__IPCTL_ATTR_MAX - 1)
  enum {
        IPCTL_CMD_UNSPEC,
        IPCTL_CMD_SET,
        IPCTL_CMD_GET,
        __IPCTL_CMD_MAX,
  };



 static int parse_cb(struct nl_msg *msg, void *arg)
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


void send_IPCTL(struct nl_sock *sock, int family, int ifIndex, int proxyArpOn)
{
 // Construct a generic netlink by allocating a new message, fill in
 // the header and append a simple integer attribute.
 struct nl_msg *msg;
 msg = nlmsg_alloc();
 genlmsg_put(msg, NL_AUTO_PID, NL_AUTO_SEQ, family, 0, NLM_F_ECHO,
             IPCTL_CMD_SET, 1 /* version */);
 nla_put_u32(msg, IPCTL_ATTR_IFINDEX, ifIndex);
 nla_put_u8(msg, IPCTL_ATTR_VALUE, proxyArpOn);

 // Send message over netlink socket
 nl_send_auto_complete(sock, msg);

 // Free message
 nlmsg_free(msg);

 // Prepare socket to receive the answer by specifying the callback
 // function to be called for valid messages.
 nl_socket_modify_cb(sock, NL_CB_VALID, NL_CB_CUSTOM, parse_cb, NULL);

 // Wait for the answer and receive it
 nl_recvmsgs_default(sock);
}


int main( int argc, const char* argv[] )
{
//	printf( "\nHello World\n\n" );

  if (argc < 3)
  {
    printf( "Usage:\n" );
    printf( "  argv[0] <ifindex> <proxy-arp-status>\n" );
    return 0; 
  }

  int ifIndex = atoi(argv[1]);
  int proxyArpOn = atoi(argv[2]);


 struct nl_sock *sock;

 int family;

 // Allocate a new netlink socket
 sock = nl_socket_alloc();

 // Connect to generic netlink socket on kernel side
 genl_connect(sock);

 // Ask kernel to resolve family name to family id
 family = genl_ctrl_resolve(sock, "ARPCONTROL");

  for (int i = 0; i < 100000; i++)
    send_IPCTL(sock, family, ifIndex, proxyArpOn);
}



