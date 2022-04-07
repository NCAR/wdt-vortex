/*
 * wdt.h: Watchdog Timer Linux Device Driver
 *
 * (C) Copyright 2012, 2016 by WinSystems, Inc.
 * Author: Paul DeMetrotion <pdemetrotion@winsystems.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 */

#ifndef __WDT_H
#define __WDT_H

#include <linux/ioctl.h> 

#define IOCTL_NUM 'd'

#define SUCCESS 0

// Read the WDT
#define IOCTL_READ_WDT _IOWR(IOCTL_NUM, 1, int)

// Write to the WDT
#define IOCTL_WRITE_WDT _IOWR(IOCTL_NUM, 2, int)

// Set seconds
#define IOCTL_SET_WDT_SEC _IOWR(IOCTL_NUM, 3, int)

// Set minutes
#define IOCTL_SET_WDT_MIN _IOWR(IOCTL_NUM, 4, int)

#endif /* __WDT_H */
