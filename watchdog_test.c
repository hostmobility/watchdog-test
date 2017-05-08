/**
 *
 * wathchdog-test - program to test Linux watchdog (/dev/watchdog)
 *
 * Copyright (C) 2017 Host Mobility AB 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/watchdog.h>

int fd;

/*
 * This function simply sends an IOCTL to the driver, which in turn ticks
 * th Watchdog to reset its internal timer so it doesn't trigger
 * a reset.
 */
static void keep_alive(void)
{
    int dummy;

    ioctl(fd, WDIOC_KEEPALIVE, &dummy);
}

static void term(int sig)
{
    close(fd);
    printf("Stopping watchdog ticks...\n");
    exit(0);
}

static void print_usage_and_exit()
{
    printf("usage: watchdog-test: \n");
    printf("\t-t <n> to set the timeout\n");
    printf("\t-p <n> to set the ping rate\n");
	exit(-1);
}

int main(int argc, char *argv[])
{
    int timeout, ret;
    unsigned int ping_rate = 1;

    fd = open("/dev/watchdog", O_WRONLY);

    if (fd == -1) {
		printf("Watchdog device not enabled.\n");
		exit(-1);
    }

    if (argc > 1) {
		if (!strncasecmp(argv[1], "-t", 2) && argv[2]) {
		    timeout = atoi(argv[2]);
		    ret = ioctl(fd, WDIOC_SETTIMEOUT, &timeout);

		    if (ret < 0) {
		    	printf("Failed to set watchdog timeout (WDIOC_SETTIMEOUT) "
                    "to %d seconds: %s\n", timeout, strerror(errno));
		    	goto end;
		    }
		    printf("Watchdog timeout set to %u seconds.\n", timeout);
		} else if (!strncasecmp(argv[1], "-p", 2) && argv[2]) {
		    ping_rate = strtoul(argv[2], NULL, 0);
		    printf("Watchdog ping rate set to %u seconds.\n", ping_rate);
		} else {
			print_usage_and_exit();
		}
    }

    ret = ioctl(fd, WDIOC_GETTIMEOUT, &timeout);
    if (ret < 0) {
        printf("Failed to get watchdog timeout (WDIOC_GETTIMEOUT)\n");
        goto end;
    }

    printf("Watchdog Ticking Away!\n");
    printf("Timeout: %d, Ping rate: %d\n", timeout, ping_rate);
    signal(SIGINT, term);

    while(1) {
		keep_alive();
		sleep(ping_rate);
    }
end:
    close(fd);
    return 0;
}
