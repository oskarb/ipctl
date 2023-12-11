#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT(ipctl, 1.1, oskar.berggren@gmail.com)

#AC_CONFIG_SRCDIR([src/ipctlm.c])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_HEADERS(config.h)

AM_INIT_AUTOMAKE()

dnl Additional arguments
dnl Kernel build directory or source tree
AC_ARG_WITH([kbuild],
            AS_HELP_STRING([--with-kbuild=PATH],
                           [Path to kernel build directory]),
            [KBUILDDIR="$withval";])
AC_ARG_WITH([ksource],
            AS_HELP_STRING([--with-ksource=PATH],
                           [Path to kernel source directory, if not the same as the kernel build directory]),
            [KSOURCEDIR="$withval";])
AM_CONDITIONAL(WITH_KBUILDDIR, test "$KBUILDDIR" != "")
AC_SUBST(KBUILDDIR)

# Checks for programs.
AC_PROG_CC_C99
LT_INIT

# Checks for libraries.
PKG_CHECK_MODULES(LIBNL3_GENL, libnl-genl-3.0 >= 3.1)

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.




AC_OUTPUT(Makefile src/cli/Makefile src/libipctl/Makefile include/libipctl/Makefile)

