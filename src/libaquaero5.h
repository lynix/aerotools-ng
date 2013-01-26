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
#include <sys/ioctl.h>
#include <linux/version.h>
#include <linux/hiddev.h>

/* constants */
#define AQ5_DATA_LEN	659
#define AQ5_CURRENT_TIME_OFFS	0x001
#define AQ5_SERIAL_MAJ_OFFS	0x007
#define AQ5_SERIAL_MIN_OFFS	0x009
#define AQ5_FIRMWARE_VER_OFFS	0x00b
#define AQ5_BOOTLOADER_VER_OFFS	0x00d
#define AQ5_HARDWARE_VER_OFFS	0x00f
#define AQ5_UPTIME_OFFS	0x011
#define AQ5_TOTAL_TIME_OFFS	0x015
#define AQ5_TEMP_OFFS	0x067
#define AQ5_TEMP_DIST	2
#define AQ5_TEMP_UNDEF	0x7fff

#define AQ5_FAN_OFFS	0x169
#define AQ5_FAN_DIST	8
#define AQ5_FAN_VRM_OFFS	0x0bf
#define AQ5_FAN_VRM_DIST	2
#define AQ5_FAN_VRM_UNDEF	0x7fff

#define AQ5_FLOW_OFFS	0x0fb
#define AQ5_FLOW_DIST	2

#define AQ5_CPU_TEMP_OFFS	0x0d7
#define AQ5_CPU_TEMP_DIST	2

#define AQ5_LEVEL_OFFS	0x147
#define AQ5_LEVEL_DIST	2

#define AQ5_NUM_TEMP	44
#define AQ5_NUM_FAN	12
#define AQ5_NUM_FLOW	14
#define AQ5_NUM_CPU	8
#define AQ5_NUM_LEVEL	4

#define AQ_TEMP_UNDEF	-99.0

typedef struct {
	uint32_t	current_time;
	uint16_t	serial_major;
	uint16_t	serial_minor;
	uint16_t	firmware_version;
	uint16_t	bootloader_version;
	uint16_t	hardware_version;
	uint32_t	uptime;
	uint32_t	total_time;
	double		temp[AQ5_NUM_TEMP];
	double		fan_current[AQ5_NUM_FAN];
	uint16_t	fan_rpm[AQ5_NUM_FAN];
	double		fan_duty_cycle[AQ5_NUM_FAN];
	double		fan_voltage[AQ5_NUM_FAN];
	double		fan_vrm_temp[AQ5_NUM_FAN];
	double		flow[AQ5_NUM_FLOW];
	double		cpu_temp[AQ5_NUM_CPU];
	double		level[AQ5_NUM_LEVEL];
} aq5_data_t;


int libaquaero5_poll(char *device, aq5_data_t *data_dest);

/* Helpful for debugging */
unsigned char *aquaero_get_buffer();

#endif /* LIBAQUAERO5_H_ */
