AUTOMAKE_OPTIONS = foreign
SUBDIRS = src/libipctl include/libipctl src/cli


if ! WITH_KBUILDDIR
KBUILD_OUTPUT=/lib/modules/`uname -r`/build
else
KBUILD_OUTPUT=$(KBUILDDIR)
endif

modules:
	${MAKE} -C $(KBUILD_OUTPUT) M=$(PWD)/src/module modules

modules_clean:
	${MAKE} -C $(KBUILD_OUTPUT) M=$(PWD)/src/module clean

modules_install:
	${MAKE} -C $(KBUILD_OUTPUT) M=$(PWD)/src/module modules_install

all: modules
#clean: modules_clean


EXTRA_DIST = src/module/ipctlm.c src/module/Kbuild src/module/Makefile bootstrap \
      README.md

