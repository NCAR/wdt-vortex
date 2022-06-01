# MAKEFILE =      Makefile
# ALL:    build
PREFIX ?= /usr

KERNEL_DIR ?= $(shell find /usr/src -maxdepth 1 -mindepth 1 -type d -print | head -n 1)
# KERNEL_DIR := /usr/src/linux-headers-4.15.18-vortex86dx3 

$(info DESTDIR=$(DESTDIR))
$(info KERNEL_DIR=$(KERNEL_DIR))
$(info DEB_BUILD_GNU_TYPE=$(DEB_BUILD_GNU_TYPE))
$(info DEB_HOST_GNU_TYPE=$(DEB_HOST_GNU_TYPE))
$(info DEB_HOST_MULTIARCH=$(DEB_HOST_MULTIARCH))

.PHONY: build
build:
	@echo "Building wdt-vortex..."
	$(MAKE) -C src KERNEL_DIR=$(KERNEL_DIR)

.PHONY: install
install:
	@echo "Installing wdt-vortex..."
	$(MAKE) -C src KERNEL_DIR=$(KERNEL_DIR) INSTALL_MOD_PATH=$(DESTDIR) modules_install
	$(MAKE) -C src DESTDIR=$(DESTDIR) PREFIX=$(PREFIX) install

.PHONY: clean
clean:
	@echo "Clean wdt-vortex..."
	$(MAKE) -C src clean

