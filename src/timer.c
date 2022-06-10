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
#include <poll.h>
#include <termios.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

#include "wdt.h"

int read_wdt(void);
int write_wdt(int);
int set_wdt_sec(void);
int set_wdt_min(void);
int check_handle(void);

struct pollfd poll_fds[1];

struct termios stdin_termios;

extern char *device_id;

/**
 * Restore original termios.
 */
void close_keyboard(void)
{
    printf("re-enabling canonical input\n");
    if (tcsetattr(0, TCSAFLUSH, &stdin_termios) < 0) {
        fprintf(stderr, "tcsetattr stdin %s\n",
                strerror(errno));
    }
}

void disable_wdt(void)
{
    printf("disabling watchdog\n");
    write_wdt(0);
}

/**
 * Turn off canonical, line-oriented, input, to be able to read
 * each keystroke.
 */
void init_keyboard(void)
{
    if (tcgetattr(0, &stdin_termios) < 0) {
        fprintf(stderr, "tcgetattr stdin: %s\n",
                strerror(errno));
    }
    struct termios ts = stdin_termios;

    ts.c_lflag &= ~ICANON;
    ts.c_cc[VMIN] = 0;
    ts.c_cc[VTIME] = 0;

    if (tcsetattr(0, TCSAFLUSH, &ts) < 0) {
        fprintf(stderr, "tcsetattr stdin: %s\n",
                strerror(errno));
    }
    atexit(close_keyboard);
}

int kbhit()
{
	int ret;
        /* timeout=0, return immediately */
	if ((ret = poll(poll_fds, 1, 0)) < 0) {
	    fprintf(stderr, "stdin poll: %s\n", strerror(errno));
	    exit(1);
	}
        if (poll_fds[0].revents & (POLLERR | POLLHUP | POLLNVAL)) {
            fprintf(stderr,"stdin poll failed\n");
            exit(1);
        }
	return ret;
}

int readch() {
	char c;
	if (read(0, &c, 1) < 0) {
	    fprintf(stderr, "stdin read: %s\n", strerror(errno));
	    exit(1);
	}
	return c;
}

void sigfunc(int sig, siginfo_t* si,void * ptr)
{
        fprintf(stderr,"signal %s(%d) received\n",  strsignal(sig), sig);
        exit(1);
}

int main(int argc, char *argv[])
{
	int timeout, key;

	// check syntax and availability
	if (argc != 3)
	{
		fprintf(stderr, "Usage: %s <time> <sec|min>\n", argv[0]);
		fprintf(stderr, "Example: %s 100 sec\n", argv[0]);
		exit(1);
	}
	else if(read_wdt() < 0)
	{	
	    fprintf(stderr, "%s: open/read: %s\n", device_id, strerror(errno));
	    exit(1);
	}
	else
	{
		printf("Test program WinSystems Watchdog Timer\n");
		timeout = atoi(argv[1]);
		printf("Settings: %d %s\n", timeout, argv[2]);
		printf("Hit any key to start program\n");
		init_keyboard();
	}

        struct sigaction sigact = {
            .sa_sigaction = sigfunc,
            .sa_flags = SA_SIGINFO
        };

        if (sigaction(SIGINT, &sigact, NULL) < 0 ||
                    sigaction(SIGTERM, &sigact, NULL) < 0) {
            fprintf(stderr, "sigaction %s\n", strerror(errno));
            exit(1);
        }

	poll_fds[0].fd = 0;
	poll_fds[0].events = POLLIN;

	// wait for keystroke
	while (!kbhit()) sleep(1);
	readch();

        printf("Enter q to stop watchdog and quit\n");

	// configure wdt
	if (*argv[2] == 's')
		set_wdt_sec();
	else
		set_wdt_min();

	write_wdt(timeout);
        atexit(disable_wdt);

	while(1)
	{
		printf("***** Current count = %d *****\n", read_wdt());

		if (kbhit())
		{
			key = readch();
			if (key == 'q')	// exit
			{
				exit(0);
			}
			else {
                            // reset wdt
                            write_wdt(timeout);
                        }
		}

		sleep(1);	// wait 1 sec
	}
}

