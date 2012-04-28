#ifndef IPCTL_NL_H_INCLUDED
#define IPCTL_NL_H_INCLUDED

/* Copyright (C) 2012 Oskar Berggren <oskar.berggren@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */


/* NETLINK attributes */
enum {
  IPCTL_ATTR_UNSPEC,
  IPCTL_ATTR_PROPERTY,   /* Use IPCTL_PROPERTY_*  */
  IPCTL_ATTR_IFINDEX,
  IPCTL_ATTR_VALUE,
  __IPCTL_ATTR_MAX,
};
#define IPCTL_ATTR_MAX (__IPCTL_ATTR_MAX - 1)


/* NETLINK commands */
enum {
  IPCTL_CMD_UNSPEC,
  IPCTL_CMD_SET,
  IPCTL_CMD_GET,
  __IPCTL_CMD_MAX,
};
#define IPCTL_CMD_MAX (__IPCTL_CMD_MAX - 1)


/* Values for IPCTL_ATTR_PROPERTY. */
enum {
  IPCTL_PROPERTY_PROXYARP,
};


/* ipctl use the generic netlink facility. */
#define IPCTL_GENL_NAME "ipctl"

#define IPCTL_GENL_VERSION 1

#endif
