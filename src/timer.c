/*
 * timer.c: Watchdog Timer Linux Device Driver
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
#include <stdlib.h>
#include <unistd.h>

#include "wdt.h"

void init_keyboard(void);
void close_keyboard(void);
int kbhit(void);
int readch(void);

main(int argc, char *argv[])
{
	int timeout, key;

	// check syntax and availability
	if (argc != 3)
	{
		printf("Usage: timer <time> <sec/min>\n");
		printf("Example: timer 100 sec\n");
		exit(1);
	}
	else if(read_wdt() < 0)
	{	
	    fprintf(stderr, "Can't access device WDT - Aborting\n");
	    exit(1);
	}
	else
	{
		printf("Test program WinSystems Watchdog Timer\n");
		timeout = atoi(argv[1]);
		printf("  Settings: %d %s\n", timeout, argv[2]);
		printf("  Hit any key to start program\n");
		init_keyboard();
	}

	// wait for keystroke
	while (!kbhit()) ;
	readch();

	// configure wdt
	if (*argv[2] == 's')
		set_wdt_sec();
	else
		set_wdt_min();

	write_wdt(timeout);

	while(1)
	{
		printf("  ***** Current count = %d *****\n", read_wdt());

		if (kbhit())
		{
			key = readch();
			if (key == 'q')	// exit
			{
				write_wdt(0);	// disable wdt
				close_keyboard();
				exit(0);
			}
			else	// reset wdt
				write_wdt(timeout);
		}

		usleep(1000000);	// wait 1 sec
	}
}
