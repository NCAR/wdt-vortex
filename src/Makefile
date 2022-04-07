# Makefile for wdt

ifneq ($(KERNELRELEASE),) # called by kbuild
	obj-m := wdt.o
else # called from command line
	KERNEL_VERSION = `uname -r`
	KERNELDIR := /lib/modules/$(KERNEL_VERSION)/build
	PWD  := $(shell pwd)
	MODULE_INSTALLDIR = /lib/modules/$(KERNEL_VERSION)/kernel/drivers/watchdog/

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

wdtio.o: wdtio.c wdt.h Makefile
	gcc -c wdtio.c

all:    default install timer

install:
	mkdir -p $(MODULE_INSTALLDIR)
	rm -f $(MODULE_INSTALLDIR)wdt.ko
	install -c -m 0644 wdt.ko $(MODULE_INSTALLDIR)
	/sbin/depmod -a

uninstall:
	rm -f $(MODULE_INSTALLDIR)wdt.ko
	/sbin/depmod -a

timer: timer.c wdt.h wdtio.o Makefile kbhit.c
	gcc -static timer.c kbhit.c wdtio.o -o timer
	chmod a+x timer

endif
 
clean:
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions /dev/wdt? Module.symvers

spotless:
	rm -rf timer Module.* *.o *~ core .depend .*.cmd *.ko *.mod.c *.order .tmp_versions /dev/wdt?
