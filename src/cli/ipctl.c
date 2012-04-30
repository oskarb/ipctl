#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <stdio.h>
#include <libipctl/ipctl.h>


int main( int argc, const char* argv[] )
{
	if (argc < 3 || strcmp(argv[2], "proxy-arp") != 0)
	{
		printf( "Usage:\n" );
		printf( "  %s <ifindex> proxy-arp <proxy-arp-status>\n", argv[0]);
		return 0; 
	}

	int ifIndex = atoi(argv[1]);
	int proxyArpOn = 0;
	int cmdIsSet = 0;
	if (argc > 3)
	{
		cmdIsSet = 1;
		proxyArpOn = atoi(argv[3]);
		proxyArpOn = proxyArpOn ? 1 : 0; // Normalize to 0 or 1.
	}

	struct nl_sock *sock;

	int family;

	// Allocate a new netlink socket
	sock = nl_socket_alloc();

	//struct nl_cb *cb = nl_cb_alloc(NL_CB_DEBUG);
	//sock = nl_socket_alloc_cb(cb);

	// Connect to generic netlink socket on kernel side
	genl_connect(sock);

	// Ask kernel to resolve family name to family id
	family = ipctl_lookup_genl_family(sock);

	if (cmdIsSet)
	{
		int rc = ipctl_set_proxy_arp(sock, family, ifIndex, proxyArpOn);
		if (rc)
			fprintf(stderr, "Error: %d %s\n", rc, nl_geterror(rc));
	}
	else 
	{
		int response = -1;
		int rc = ipctl_get_proxy_arp(sock, family, ifIndex, &response);
		if (rc)
			fprintf(stderr, "Error: %d %s\n", rc, nl_geterror(rc));
		else
			printf("%d\n", response);
	}

	nl_socket_free(sock);
}

