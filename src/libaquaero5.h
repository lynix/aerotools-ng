/* Copyright 2012 lynix <lynix47@gmail.com>
 * Copyright 2013 JinTu <JinTu@praecogito.com>, lynix <lynix47@gmail.com>
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

#include <stdint.h>

/* sensor quantity */
#define AQ5_NUM_TEMP			44
#define AQ5_NUM_FAN				12
#define AQ5_NUM_FLOW			14
#define AQ5_NUM_CPU				8
#define AQ5_NUM_LEVEL			4


/* constant for unknown value */
#define AQ5_FLOAT_UNDEF			-99.0


/* structures holding device data */
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
	uint16_t	fan_current[AQ5_NUM_FAN];
	uint16_t	fan_rpm[AQ5_NUM_FAN];
	double		fan_duty_cycle[AQ5_NUM_FAN];
	double		fan_voltage[AQ5_NUM_FAN];
	double		fan_vrm_temp[AQ5_NUM_FAN];
	double		flow[AQ5_NUM_FLOW];
	double		cpu_temp[AQ5_NUM_CPU];
	double		level[AQ5_NUM_LEVEL];
} aq5_data_t;

typedef struct {
	uint16_t	fan_min_rpm[AQ5_NUM_FAN];
	uint16_t	fan_max_rpm[AQ5_NUM_FAN];
	double		fan_max_duty_cycle[AQ5_NUM_FAN];
	double		fan_min_duty_cycle[AQ5_NUM_FAN];
	double		fan_startboost_duty_cycle[AQ5_NUM_FAN];
	uint16_t	fan_startboost_duration[AQ5_NUM_FAN];
	uint16_t	fan_pulses_per_revolution[AQ5_NUM_FAN];
	/* unknown 1 */
	/* unknown 2 */
	uint16_t	fan_programmable_fuse[AQ5_NUM_FAN];
} aq5_settings_t;


/* functions to be called from application */
int		libaquaero5_poll(char *device, aq5_data_t *data_dest, char **err_msg);
int		libaquaero5_getsettings(char *device, aq5_settings_t *settings_dest, char **err_msg);
void	libaquaero5_exit();

/* helpful for debugging */
int 	libaquaero5_dump_data(char *file);
int		libaquaero5_dump_settings(char *file);

#endif /* LIBAQUAERO5_H_ */
