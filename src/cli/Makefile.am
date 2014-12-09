AM_CPPFLAGS = -I$(top_srcdir)/include $(LIBNL3_GENL_CFLAGS)

sbin_PROGRAMS = ipctl
ipctl_SOURCES = ipctl.c
ipctl_LDFLAGS = $(LIBNL3_GENL_LIBS)
ipctl_LDADD = ../libipctl/libipctl.la

