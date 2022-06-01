# Makefile for wdt

ifneq ($(KERNELRELEASE),) # called by kbuild

$(info KERNELRELEASE is not empty)

obj-m := wdt_vortex.o

else # called from command line

$(info ifneq KERNELRELEASE is empty)

# if KERNEL_DIR is empty, look on /usr/src
ifeq ($(KERNEL_DIR),)
KERNEL_DIR := $(shell find /usr/src -maxdepth 1 -mindepth 1 -type d -print | head -n 1)
endif

# if KERNEL_DIR is still empty, get current kernel version - assumes we're building
# for this host system.
ifeq ($(KERNEL_DIR),)
KERNEL_DIR := /lib/modules/$(shell uname -r)/build
endif

$(info KERNEL_DIR=$(KERNEL_DIR))

# KERNEL_VERSION = `uname -r`
# KERNEL_DIR := /lib/modules/$(KERNEL_VERSION)/build
# PWD  := $(shell pwd)
# MODULE_INSTALLDIR = /lib/modules/$(KERNEL_VERSION)/kernel/drivers/watchdog/

default: wdt_test modules
#	$(MAKE) -C $(KERNEL_DIR) M=$$PWD

endif

all: modules wdt_test

wdtio.o: wdtio.c wdt.h Makefile
	gcc -c wdtio.c

modules modules_install clean::
	$(RM) -rf *.o.ur-safe *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions Module.symvers wdt_test
	@$(MAKE) -C $(KERNEL_DIR) M=$$PWD $@

install: wdt_test
	install -d $(DESTDIR)$(PREFIX)/bin
	install wdt_test $(DESTDIR)$(PREFIX)/bin

uninstall:
	rm -f $(MODULE_INSTALLDIR)wdt_vortex.ko
	rm -f $(DESTDIR)$(PREFIX)/bin/wdt_test
	/sbin/depmod -a

wdt_test: timer.c wdt.h wdtio.o Makefile
	gcc -static timer.c wdtio.o -o wdt_test
	chmod a+x wdt_test

spotless:
	rm -rf wdt_test Module.* *.o *~ core .depend .*.cmd *.ko *.mod.c *.order .tmp_versions /dev/wdt?
