#ifndef IPCTL_H_INCLUDED
#define IPCTL_H_INCLUDED

/* Copyright (C) 2012 Oskar Berggren <oskar.berggren@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */


extern int
ipctl_lookup_genl_family(struct nl_sock *socket);

extern int
ipctl_set_proxy_arp(struct nl_sock *socket, int family, int ifIndex, int on);

extern int
ipctl_get_proxy_arp(struct nl_sock *socket, int family, int ifIndex, int *response);


#endif

