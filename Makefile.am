AUTOMAKE_OPTIONS = foreign
SUBDIRS = src/libipctl include/libipctl src/cli


modules:
	${MAKE} -C /lib/modules/$(shell uname -r)/build M=$(PWD)/src/module modules

modules_clean:
	${MAKE} -C /lib/modules/$(shell uname -r)/build M=$(PWD)/src/module clean


all: modules
#clean: modules_clean
