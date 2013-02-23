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
#define AQ5_NUM_FAN			12
#define AQ5_NUM_FLOW			14
#define AQ5_NUM_CPU			8
#define AQ5_NUM_LEVEL			4
#define AQ5_NUM_INFO_PAGE		32


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
	boolean_t		use_startboost;
	boolean_t		hold_minimum_power;
	boolean_t		use_programmable_fuse;
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

typedef enum { 
	POINT		= 0x00, 
	COMMA		= 0x01 
} decimal_separator_t;

typedef enum {
	LOG_DATA_1	= 0x00,
	LOG_DATA_2	= 0x01,
	LOG_DATA_3	= 0x02,
	LOG_DATA_4	= 0x03,
	LOG_DATA_5	= 0x04,
	LOG_DATA_6	= 0x05,
	LOG_DATA_7	= 0x06,
	LOG_DATA_8	= 0x07,
	LOG_DATA_9	= 0x08,
	LOG_DATA_10	= 0x09,
	LOG_DATA_11	= 0x0a,
	LOG_DATA_12	= 0x0b,
	LOG_DATA_13	= 0x0c,
	LOG_DATA_14	= 0x0d,
	LOG_DATA_15	= 0x0e,
	LOG_DATA_16	= 0x0f,
	SENSOR_1_2	= 0x10,
	SENSOR_3_4	= 0x11,
	SENSOR_5_6	= 0x12,
	SENSOR_7_8	= 0x13,
	POWERADJUST_1_2	= 0x14,
	POWERADJUST_3_4	= 0x15,
	POWERADJUST_5_6	= 0x16,
	POWERADJUST_7_8	= 0x17,
	SOFT_SENSOR_1_2	= 0x18,
	SOFT_SENSOR_3_4	= 0x19,
	SOFT_SENSOR_5_6	= 0x1a,
	SOFT_SENSOR_7_8	= 0x1b,
	VIRT_SENSOR_1_2	= 0x1c,
	VIRT_SENSOR_3_4	= 0x1d,
	MPS_1		= 0x1e,
	MPS_2		= 0x1f,
	MPS_3		= 0x20,
	MPS_4		= 0x21,
	AQUASTREAM_XT	= 0x22,
	SENSOR_39_40	= 0x23,
	SENSOR_1_4	= 0x24,
	SENSOR_5_8	= 0x25,
	POWERADJUST_1_4	= 0x26,
	POWERADJUST_5_8 = 0x27,
	SOFT_SENSOR_1_4	= 0x28,
	SOFT_SENSOR_5_8	= 0x29,
	VIRT_SENSORS	= 0x2a,
	MPS_1_2		= 0x2b,
	MPS_3_4		= 0x2c,
	AQUASTREAM	= 0x2d,
	SENSOR_41_44	= 0x2e,
	FAN_AMP_1_4	= 0x2f,
	FAN_AMP_5_8	= 0x30,
	FAN_AMP_9_12	= 0x31,
	SENSOR_56_60	= 0x32,
	SENSOR_61_64	= 0x33,
	FAN_1_4_POWER	= 0x34,
	FAN_5_8_POWER	= 0x35,
	FAN_9_12_POWER	= 0x36,
	FAN_1_4_RPM	= 0x37,
	FAN_5_8_RPM	= 0x38,
	FAN_9_12_RPM	= 0x39,
	FAN_1		= 0x3a,
	FAN_2		= 0x3b,
	FAN_3		= 0x3c,
	FAN_4		= 0x3d,
	FAN_5		= 0x3e,
	FAN_6		= 0x3f,
	FAN_7		= 0x40,
	FAN_8		= 0x41,
	FAN_9		= 0x42,
	FAN_10		= 0x43,
	FAN_11		= 0x44,
	FAN_12		= 0x45,
	FLOW_1		= 0x46,
	FLOW_2		= 0x47,
	FLOW_3		= 0x48,
	FLOW_4		= 0x49,
	FLOW_5		= 0x4a,
	FLOW_6		= 0x4b,
	FLOW_7		= 0x4c,
	FLOW_8		= 0x4d,
	FLOW_9		= 0x4e,
	FLOW_10		= 0x4f,
	FLOW_11		= 0x50,
	FLOW_12		= 0x51,
	FLOW_13		= 0x52,
	FLOW_14		= 0x53,
	AQUASTREAM_XT_1	= 0x54,
	AQUASTREAM_XT_2	= 0x55,
	MULTISWITCH_1	= 0x56,
	MULTISWITCH_2	= 0x57,
	FILL_LEVEL_1	= 0x58,
	FILL_LEVEL_2	= 0x59,
	FILL_LEVEL_3	= 0x5a,
	FILL_LEVEL_4	= 0x5b,
	PWR_CONSUMPT_1	= 0x5c,
	PWR_CONSUMPT_2	= 0x5d,
	PWR_CONSUMPT_3	= 0x5e,
	PWR_CONSUMPT_4	= 0x5f,
	AQUAERO_INFO	= 0x60,
	TIME		= 0x61,
	USB_LCD_PAGE	= 0x62,
	RELAY_AND_POWER	= 0x63,
	USER_DEF_LOGO	= 0x64
} info_page_type_t;

typedef enum {
	STANDBY_ACTION,
	DATE_FORMAT,
	TIME_FORMAT,
	AUTO_DST,
	DISPLAY_MODE,
	FAN_DATA_SRC, 
	LANGUAGE, 
	TEMP_UNITS, 
	FLOW_UNITS, 
	PRESSURE_UNITS, 
	DECIMAL_SEPARATOR,
	INFO_SCREEN, 
	PAGE_DISPLAY_MODE,
	DISABLE_KEYS,
	ILLUM_MODE,
	KEY_TONE
} val_str_opt_t;

typedef enum {
	HIDE_PAGE	= 0x00,
	SHOW_PAGE	= 0x01,
	SHOW_PAGE_PERM	= 0x02
} page_display_mode_t;

typedef enum {
	ENABLED		= 0x00,
	DISABLED	= 0xFF
} disable_keys_t;

typedef enum {
	AUTO_OFF	= 0x00,
	ALWAYS_ON	= 0x01
} illumination_mode_t;

typedef enum {
	OFF		= 0x00,
	QUIET		= 0x01,
	NORMAL		= 0x02,
	LOUD		= 0x03
} key_tone_t;

typedef struct {
	page_display_mode_t	page_display_mode;
	uint16_t		display_time;
	info_page_type_t	info_page_type;
} info_page_t;

typedef struct {
	uint16_t	enter_key;
	uint16_t	up_key;
	uint16_t	down_key;
	uint16_t	programmable_key_1;
	uint16_t	programmable_key_2;
	uint16_t	programmable_key_3;
	uint16_t	programmable_key_4;
} key_sensitivity_t;	

typedef enum {
	YEAR_MONTH_DAY	= 0x00,
	DAY_MONTH_YEAR	= 0x04
} date_format_t;

typedef enum {
	TWELVE_HOUR		= 0x00,
	TWENTY_FOUR_HOUR	= 0x02
} time_format_t;

typedef enum {
	DST_DISABLED	= 0x00,
	DST_ENABLED	= 0x01
} auto_dst_t;

typedef enum {
	DISP_NORMAL	= 0x00,
	DISP_INVERTED	= 0x01
} display_mode_t;	

typedef enum {
	NO_ACTION		= 0x00,
	SPEED_SIG_GEN_ON	= 0x00,
	SPEED_SIG_GEN_OFF	= 0x00,
	ALARM_BUZZER_ON		= 0x00,
	ALARM_BUZZER_OFF	= 0x00,
	ALARM_BUZZER_CYCLE_ON_OFF	= 0x00,
	ALARM_BUZZER_SINGLE_TONE	= 0x00,
	RELAY_ON		= 0x00,
	RELAY_OFF		= 0x00,
	SWITCH_RELAY_2_SEC	= 0x00,
	SWITCH_RELAY_10_SEC	= 0x00,
	LOAD_PROFILE_1		= 0x00,
	LOAD_PROFILE_2		= 0x00,
	LOAD_PROFILE_3		= 0x00,
	LOAD_PROFILE_4		= 0x00,
	USB_KEYBD_POWER_KEY	= 0x00,
	USB_KEYBD_SLEEP_KEY	= 0x00,
	USB_KEYBD_WAKEUP_KEY	= 0x00
} standby_action_t;

typedef struct {
	double			standby_display_contrast;
	double			standby_lcd_backlight_brightness;
	double			standby_key_backlight_brightness;
	uint16_t		standby_timeout_key_and_display_brightness;
	standby_action_t	standby_action_at_power_down;
	standby_action_t	standby_action_at_power_up;
	time_format_t		time_format;
	date_format_t		date_format;
	auto_dst_t		auto_dst;
	int8_t			time_zone;
	double			display_contrast;
	double			display_brightness_while_in_use;
	double			display_brightness_while_idle;
	uint16_t		display_illumination_time;
	illumination_mode_t	display_illumination_mode;
	display_mode_t		display_mode;
	uint16_t		menu_display_duration;
	uint16_t		display_duration_after_page_selection;
	disable_keys_t		disable_keys;
	double			brightness_while_in_use;
	double			brightness_while_idle;
	illumination_mode_t	illumination_mode;
	key_tone_t		key_tone;
	key_sensitivity_t	key_sensitivity;
	temp_units_t		temp_units;
	flow_units_t		flow_units;
	pressure_units_t	pressure_units;
	decimal_separator_t	decimal_separator;
	language_t		language;
	info_page_t		info_page[AQ5_NUM_INFO_PAGE];	
	double			temp_offset[AQ5_NUM_TEMP];
	double			fan_vrm_temp_offset[AQ5_NUM_TEMP];
	double			cpu_temp_offset[AQ5_NUM_TEMP];
	uint16_t		fan_min_rpm[AQ5_NUM_FAN];
	uint16_t		fan_max_rpm[AQ5_NUM_FAN];
	double			fan_max_duty_cycle[AQ5_NUM_FAN];
	double			fan_min_duty_cycle[AQ5_NUM_FAN];
	double			fan_startboost_duty_cycle[AQ5_NUM_FAN];
	uint16_t		fan_startboost_duration[AQ5_NUM_FAN];
	uint16_t		fan_pulses_per_revolution[AQ5_NUM_FAN];
	aq5_fan_control_mode_t	fan_control_mode[AQ5_NUM_FAN];
	fan_data_source_t	fan_data_source[AQ5_NUM_FAN];
	uint16_t		fan_programmable_fuse[AQ5_NUM_FAN];
} aq5_settings_t;


/* functions to be called from application */
int	libaquaero5_poll(char *device, aq5_data_t *data_dest, char **err_msg);
int	libaquaero5_getsettings(char *device, aq5_settings_t *settings_dest, char **err_msg);
void	libaquaero5_exit();
char	*libaquaero5_get_string(int id, val_str_opt_t opt);

/* helpful for debugging */
int 	libaquaero5_dump_data(char *file);
int	libaquaero5_dump_settings(char *file);

#endif /* LIBAQUAERO5_H_ */
