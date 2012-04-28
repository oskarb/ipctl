AUTOMAKE_OPTIONS = foreign
SUBDIRS = src/cli


modules:
	${MAKE} -C /lib/modules/$(shell uname -r)/build M=$(PWD)/src/module modules

#clean:
#	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

