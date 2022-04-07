/*
 * wdtio.c: Watchdog Timer Linux Device Driver
 *
 * (C) Copyright 2012, 2016 by WinSystems, Inc.
 * Author: Paul DeMetrotion <pdemetrotion@winsystems.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 */

#include <stdio.h>
#include <fcntl.h>      /* open */ 
#include <unistd.h>     /* exit */
#include <sys/ioctl.h>  /* ioctl */

// Include the WinSystems WDT definitions
#include "wdt.h"    

//**************************************************************************
//		USER LIBRARY FUNCTIONS
//**************************************************************************

// device handles
int handle = 0;

// the names of our device nodes
char *device_id={"/dev/wdt"};

//
//------------------------------------------------------------------------
//
// read_wdt - Read the watchdog timer
//
// Description:		This function will read the address 0x566 and return
//					the current watchdog timer count
//
// Arguments:
//
// Returns:
//			-1		If the handle is invalid
//					The result of the read
//
//------------------------------------------------------------------------
//
int read_wdt(void)
{
	int c;

    if(check_handle())
		return -1;

    c = ioctl(handle, IOCTL_READ_WDT);
    
    return c;
}

//
//------------------------------------------------------------------------
//
// write_wdt - Write to the watchdog timer
//
// Description:		This function will write the address 0x566 and reset
//					the watchdog timer
//
// Arguments:
//			val		The value to write to the register
//
// Returns:
//			-1		If the handle is invalid
//			 0		The write is completed
//
//------------------------------------------------------------------------
//
int write_wdt(int value)
{
	int c;

    if(check_handle())
		return -1;

    c = ioctl(handle, IOCTL_WRITE_WDT, value);

    return c;
}

//
//------------------------------------------------------------------------
//
// set_wdt_sec - Sets the watchdog timer units to seconds
//
// Description:		This function will write 0x80 to the address 0x565 
//					and set the watchdog timer units to seconds
//
// Arguments:
//
// Returns:
//			-1		If the handle is invalid
//			 0		The units are set to seconds
//
//------------------------------------------------------------------------
//
int set_wdt_sec(void)
{
	int c;

    if(check_handle())
		return -1;

    c = ioctl(handle, IOCTL_SET_WDT_SEC);

    return c;
}

//
//------------------------------------------------------------------------
//
// set_wdt_min - Sets the watchdog timer units to minutes
//
// Description:		This function will write 0 to the address 0x565 
//					and set the watchdog timer units to minutes
//
// Arguments:
//
// Returns:
//			-1		If the handle is invalid
//			 0		The units are set to minutes
//
//------------------------------------------------------------------------
//
int set_wdt_min(void)
{
	int c;

    if(check_handle())
		return -1;

    c = ioctl(handle, IOCTL_SET_WDT_MIN);

    return c;
}

//
//------------------------------------------------------------------------
//
// check_handle - Checks that a handle to the appropriate device file
//					exists. If it does not a file open is performed.
//
// Description:
//
// Arguments:
//			chip_number	The 1 based index of the chip
//
// Returns:
//			0		if handle is valid
//			-1		If the chip does not exist or it's handle is invalid
//
//------------------------------------------------------------------------
//
int check_handle(void)
{
    if(handle > 0)	// If it's already a valid handle
		return 0;

    if(handle == -1)	// If it's already been tried
		return -1;

	// Try opening the device file, in case it hasn't been opened yet
    handle = open(device_id, O_RDWR);

    if(handle > 0)	// If it's now a valid open handle
		return 0;
    
    handle = -1;
		return -1;
}
