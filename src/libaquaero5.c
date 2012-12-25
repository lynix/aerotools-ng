/* Copyright 2012 lynix <lynix47@gmail.com>
 *
 * This file is part of aerotools-ng.
 *
 * aerotools-ng is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * aerotools-ng is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with aerotools-ng. If not, see <http://www.gnu.org/licenses/>.
 */

#include "libaquaero5.h"

/* libs */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <linux/version.h>

/* constats */
/* #define AQ5_DATA_LEN	659 */
#define AQ5_TEMP_OFFS	0x067
#define AQ5_TEMP_DIST	2
#define AQ5_TEMP_UNDEF	0x7fff
#define AQ5_FRPM_OFFS	0x169
#define AQ5_FRPM_DIST	8
#define AQ5_FLOW_OFFS	0x0fb

unsigned char buffer[AQ5_DATA_LEN];

inline int aq5_get_int(unsigned char *buffer, short offset)
{
	return (buffer[offset] << 8) | buffer[offset + 1];
}

int libaquaero5_poll(char *device, aq5_data_t *data_dest)
{
	int fd = open(device, O_RDONLY);

	if (fd < 0)
		return -1;

	/* This is needed for kernel versions < 2.6.34 */
	if (get_kernel_version() < KERNEL_VERSION(2,6,34)) {
		/* printf("Kernel version is < 2.6.34, chopping off the first byte\n"); */	
		if (read(fd, buffer, AQ5_DATA_LEN + 1) != AQ5_DATA_LEN + 1) {
			close(fd);
			return -1;
		}
		memmove(buffer, buffer + 1, AQ5_DATA_LEN + 1);
	} else {
		if (read(fd, buffer, AQ5_DATA_LEN) != AQ5_DATA_LEN) {
			close(fd);
			return -1;
		}
	}

	close(fd);

	/* temperature sensors */
	int n;
	for (int i=0; i<AQ5_NUM_TEMP; i++) {
		n = aq5_get_int(buffer, AQ5_TEMP_OFFS  + i * AQ5_TEMP_DIST);
		data_dest->temp[i] = n!=AQ5_TEMP_UNDEF ? (double)n/100.0 : AQ_TEMP_UNDEF;
	}

	/* fans */
	for (int i=0; i<AQ5_NUM_FAN; i++) {
		data_dest->fan_rpm[i] = aq5_get_int(buffer, AQ5_FRPM_OFFS +
				i * AQ5_FRPM_DIST);
	}

	/* flow sensor */
	data_dest->flow = (double)aq5_get_int(buffer, AQ5_FLOW_OFFS) / 10.0;

	return 0;
}

/* Get the kernel version */
uint32_t get_kernel_version()
{
	struct utsname name;
	int major, minor, release;
	int ret;
	static uint32_t kernel_version;

	/* Check the kernel version */
	uname(&name);
	ret = sscanf(name.release, "%d.%d.%d", &major, &minor, &release);
	if (ret == 3) {
		kernel_version = major << 16 | minor << 8 | release;
		/* printf("Kernel Version: %d\n", kernel_version); */
		return kernel_version;
	} else {
		printf("Couldn't sscanf() version string %s\n", name.release);
		return 0;
	}

}

/* Return the raw buffer data */
unsigned char *aquaero_get_buffer()
{
	return buffer;
}

