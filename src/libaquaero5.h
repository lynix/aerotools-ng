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

typedef enum { 
	M_OUTPUT 	= 0x0000, 
	M_RPM 		= 0x0001 
} fan_regulation_mode_t;

typedef enum { FALSE, TRUE } boolean_t;

typedef enum {
	NONE		=	0xffff,
	/* Target value controllers */
	TARGET_VAL_CONT_1	=	0x0040,
	TARGET_VAL_CONT_2	=	0x0041,
	TARGET_VAL_CONT_3	=	0x0042,
	TARGET_VAL_CONT_4	=	0x0043,
	TARGET_VAL_CONT_5	=	0x0044,
	TARGET_VAL_CONT_6	=	0x0045,
	TARGET_VAL_CONT_7	=	0x0046,
	TARGET_VAL_CONT_8	=	0x0047,
	/* Two point controllers */
	TWO_POINT_CONT_1	=	0x0048,
	TWO_POINT_CONT_2	=	0x0049,
	TWO_POINT_CONT_3	=	0x004a,
	TWO_POINT_CONT_4	=	0x004b,
	TWO_POINT_CONT_5	=	0x004c,
	TWO_POINT_CONT_6	=	0x004d,
	TWO_POINT_CONT_7	=	0x004e,
	TWO_POINT_CONT_8	=	0x004f,
	TWO_POINT_CONT_9	=	0x0050,
	TWO_POINT_CONT_10	=	0x0051,
	TWO_POINT_CONT_11	=	0x0052,
	TWO_POINT_CONT_12	=	0x0053,
	TWO_POINT_CONT_13	=	0x0054,
	TWO_POINT_CONT_14	=	0x0055,
	TWO_POINT_CONT_15	=	0x0056,
	TWO_POINT_CONT_16	=	0x0057,
	/* Curve controllers */
	CURVE_CTRLR_1	=	0x0058,
	CURVE_CTRLR_2	=	0x0059,
	CURVE_CTRLR_3	=	0x005a,
	CURVE_CTRLR_4	=	0x005b,
	/* RGB LEDs */
	RGB_LED_RED	=	0x005c,
	RGB_LED_BLUE	=	0x005d,
	RGB_LED_GREEN	=	0x005e,
	/* Preset values */
	PRESET_VAL_1	=	0x0060,
	PRESET_VAL_2	=	0x0061,
	PRESET_VAL_3	=	0x0062,
	PRESET_VAL_4	=	0x0063,
	PRESET_VAL_5	=	0x0064,
	PRESET_VAL_6	=	0x0065,
	PRESET_VAL_7	=	0x0066,
	PRESET_VAL_8	=	0x0067,
	PRESET_VAL_9	=	0x0068,
	PRESET_VAL_10	=	0x0069,
	PRESET_VAL_11	=	0x006a,
	PRESET_VAL_12	=	0x006b,
	PRESET_VAL_13	=	0x006c,
	PRESET_VAL_14	=	0x006d,
	PRESET_VAL_15	=	0x006e,
	PRESET_VAL_16	=	0x006f,
	PRESET_VAL_17	=	0x0070,
	PRESET_VAL_18	=	0x0071,
	PRESET_VAL_19	=	0x0072,
	PRESET_VAL_20	=	0x0073,
	PRESET_VAL_21	=	0x0074,
	PRESET_VAL_22	=	0x0075,
	PRESET_VAL_23	=	0x0076,
	PRESET_VAL_24	=	0x0077,
	PRESET_VAL_25	=	0x0078,
	PRESET_VAL_26	=	0x0079,
	PRESET_VAL_27	=	0x007a,
	PRESET_VAL_28	=	0x007b,
	PRESET_VAL_29	=	0x007c,
	PRESET_VAL_30	=	0x007d,
	PRESET_VAL_31	=	0x007e,
	PRESET_VAL_32	=	0x007f
} fan_data_source_t;

typedef struct {
	fan_regulation_mode_t	fan_regulation_mode;
	boolean_t	use_startboost;
	boolean_t	hold_minimum_power;
	boolean_t	use_programmable_fuse;
} aq5_fan_control_mode_t;

typedef enum { 
	ENGLISH		= 0x00, 
	GERMAN		= 0x01 
} language_t;

typedef enum { 
	CELSIUS		= 0x00, 
	FAHRENHEIT	= 0x01, 
	KELVIN		= 0x02 
} temp_units_t;

typedef enum { 
	LPH		= 0x00, 
	LPM		= 0x01, 
	GPH_US		= 0x02, 
	GPM_US		= 0x03,
	GPH_IMP		= 0x04,
	GPM_IMP		= 0x05
} flow_units_t;

typedef enum { 
	BAR		= 0x00, 
	PSI		= 0x01 
} pressure_units_t;

typedef struct {
	temp_units_t	temp_units;
	flow_units_t	flow_units;
	pressure_units_t	pressure_units;
	language_t	language;
	double		temp_offset[AQ5_NUM_TEMP];
	double		fan_vrm_temp_offset[AQ5_NUM_TEMP];
	double		cpu_temp_offset[AQ5_NUM_TEMP];
	uint16_t	fan_min_rpm[AQ5_NUM_FAN];
	uint16_t	fan_max_rpm[AQ5_NUM_FAN];
	double		fan_max_duty_cycle[AQ5_NUM_FAN];
	double		fan_min_duty_cycle[AQ5_NUM_FAN];
	double		fan_startboost_duty_cycle[AQ5_NUM_FAN];
	uint16_t	fan_startboost_duration[AQ5_NUM_FAN];
	uint16_t	fan_pulses_per_revolution[AQ5_NUM_FAN];
	aq5_fan_control_mode_t	fan_control_mode[AQ5_NUM_FAN];
	fan_data_source_t	fan_data_source[AQ5_NUM_FAN];
	uint16_t	fan_programmable_fuse[AQ5_NUM_FAN];
} aq5_settings_t;


/* functions to be called from application */
int	libaquaero5_poll(char *device, aq5_data_t *data_dest, char **err_msg);
int	libaquaero5_getsettings(char *device, aq5_settings_t *settings_dest, char **err_msg);
void	libaquaero5_exit();
char	*libaquaero5_get_fan_data_source_string(int id);
char	*libaquaero5_get_language_string(int id);
char	*libaquaero5_get_temp_units_string(int id);
char	*libaquaero5_get_flow_units_string(int id);
char	*libaquaero5_get_pressure_units_string(int id);

/* helpful for debugging */
int 	libaquaero5_dump_data(char *file);
int	libaquaero5_dump_settings(char *file);

#endif /* LIBAQUAERO5_H_ */
