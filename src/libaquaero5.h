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

#ifndef LIBAQUAERO5_H_
#define LIBAQUAERO5_H_

/* libs */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <linux/version.h>

/* constats */
#define AQ5_DATA_LEN	659
#define AQ5_TEMP_OFFS   0x067
#define AQ5_TEMP_DIST   2
#define AQ5_TEMP_UNDEF  0x7fff
#define AQ5_FRPM_OFFS   0x169
#define AQ5_FRPM_DIST   8
#define AQ5_FLOW_OFFS   0x0fb


#define AQ5_NUM_TEMP	8
#define AQ5_NUM_FAN		4

#define AQ_TEMP_UNDEF	-99.0


typedef struct {
	double		temp[AQ5_NUM_TEMP];
	uint16_t	fan_rpm[AQ5_NUM_FAN];
	double		flow;
} aq5_data_t;


int libaquaero5_poll(char *device, aq5_data_t *data_dest);

/* Helpful for debugging */
unsigned char *aquaero_get_buffer();

uint32_t get_kernel_version();

#endif /* LIBAQUAERO5_H_ */
