/*
 * wdt.c: Watchdog Timer Linux Device Driver
 *
 * (C) Copyright 2012, 2016 by WinSystems, Inc.
 * Author: Paul DeMetrotion <pdemetrotion@winsystems.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 */

/* Helper to format our pr_* functions */
#define pr_fmt(__fmt) KBUILD_MODNAME ": " __fmt

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>
#include <linux/io.h>
#include <linux/fs.h>

#include "wdt.h"

#define MOD_DESC "WinSystems, Inc. Watchdog Timer Driver"
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION(MOD_DESC);
MODULE_AUTHOR("Paul DeMetrotion");

// Driver major number
static int cdev_major;      // 0 = allocate dynamically
module_param(cdev_major, uint, S_IRUGO);
MODULE_PARM_DESC(cdev_major, "Set the major number for wdt device (0 = auto, default)");

// wdt char device structure
static struct cdev wdt_cdev;

// This holds the base addresses of the wdt
static unsigned base_port = 0x564;
module_param(base_port, uint, S_IRUGO);
MODULE_PARM_DESC(base_port, "Set the base port for the wdt device (default is 0x564)");

// mutex & spinlock
static DEFINE_MUTEX(mtx);
static struct class *wdt_class;
static dev_t devno;

//**********************************************************************
//			DEVICE OPEN
//**********************************************************************
// called whenever a process attempts to open the device file
static int device_open(struct inode *inode, struct file *file)
{
	pr_devel("device_open(%p)\n", file);
	return 0;
}

//**********************************************************************
//			DEVICE CLOSE
//**********************************************************************
static int device_release(struct inode *inode, struct file *file)
{
	pr_devel("device_release(%p,%p)\n", inode, file);
	return 0;
}

//**********************************************************************
//			DEVICE IOCTL
//**********************************************************************
static long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
{
	int ret = 0;

	pr_devel("IOCTL call Code %04X\n", ioctl_num);

	switch (ioctl_num) {
	case IOCTL_READ_WDT:
		ret = inb(base_port + 2);
		break;

	case IOCTL_WRITE_WDT:
		mutex_lock_interruptible(&mtx);
		outb(ioctl_param & 0xff, base_port + 2);
		mutex_unlock(&mtx);
		break;

	case IOCTL_SET_WDT_SEC:
		mutex_lock_interruptible(&mtx);
		outb(0x80, base_port + 1);
		mutex_unlock(&mtx);
		break;

	case IOCTL_SET_WDT_MIN:
		mutex_lock_interruptible(&mtx);
		outb(0, base_port + 1);
		mutex_unlock(&mtx);
		break;

	default:
		ret = -EINVAL;
	}

	return ret;
}

//**********************************************************************
//			Module Declarations
// This structure will hold the functions to be called
// when a process does something to the device
//**********************************************************************
static struct file_operations wdt_fops = {
	owner:		THIS_MODULE,
	unlocked_ioctl:	device_ioctl,
	open:		device_open,
	release:	device_release,
};

//**********************************************************************
//			INIT MODULE
//**********************************************************************
int init_module()
{
	int ret_val;

	wdt_class = class_create(THIS_MODULE, KBUILD_MODNAME);
	if (IS_ERR(wdt_class)) {
		pr_err("Could not create module class\n");
		return PTR_ERR(wdt_class);
	}

	/* Register the character device. */
	if (cdev_major) {
		devno = MKDEV(cdev_major, 0);
		ret_val = register_chrdev_region(devno, 1, KBUILD_MODNAME);
	} else {
		ret_val = alloc_chrdev_region(&devno, 0, 1, KBUILD_MODNAME);
	}

	if (ret_val < 0) {
		pr_err("Could not allocate character device\n");
		goto err_del_class;
	}

	/* Add character device. */
	cdev_init(&wdt_cdev, &wdt_fops);
	ret_val = cdev_add(&wdt_cdev, devno, 1);
	if (ret_val) {
		pr_err("Error adding character device\n");
		goto err_rel_chrdev;
	}

	/* Check and map our I/O region requests. */
	if (request_region(base_port, 4, KBUILD_MODNAME) == NULL) {
		pr_err("Unable to use I/O Address %04X\n", base_port);
		ret_val = -ENOMEM;
		goto err_rel_chrdev;
	}

	pr_info("Added device @ 0x%04x\n", base_port);

	device_create(wdt_class, NULL, devno, NULL, KBUILD_MODNAME);

	return 0;

err_rel_chrdev:
	cdev_del(&wdt_cdev);
	unregister_chrdev_region(devno, 1);
err_del_class:
	class_destroy(wdt_class);

	return ret_val;
}

//**********************************************************************
//			CLEANUP MODULE
//**********************************************************************
void cleanup_module()
{
	release_region(base_port, 4);

	device_destroy(wdt_class, devno);
	class_destroy(wdt_class);
	unregister_chrdev_region(devno, 1);

	pr_info("Removed device\n");
}
