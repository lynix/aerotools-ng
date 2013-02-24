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

#include "libaquaero5.h"

/* libs */
#include <stdio.h>

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/ioctl.h>
#include <linux/hiddev.h>
#include <dirent.h>

/* usb communication related constants */
#define AQ5_USB_VID				0x0c70
#define AQ5_USB_PID				0xf001

/* device data constants */
#define AQ5_DATA_LEN			659
#define AQ5_CURRENT_TIME_OFFS	0x001
#define AQ5_SERIAL_MAJ_OFFS		0x007
#define AQ5_SERIAL_MIN_OFFS		0x009
#define AQ5_FIRMWARE_VER_OFFS	0x00b
#define AQ5_BOOTLOADER_VER_OFFS	0x00d
#define AQ5_HARDWARE_VER_OFFS	0x00f
#define AQ5_UPTIME_OFFS			0x011
#define AQ5_TOTAL_TIME_OFFS		0x015
#define AQ5_TEMP_OFFS			0x067
#define AQ5_TEMP_DIST			2
#define AQ5_TEMP_UNDEF			0x7fff
#define AQ5_FAN_OFFS			0x169
#define AQ5_FAN_DIST			8
#define AQ5_FAN_VRM_OFFS		0x0bf
#define AQ5_FAN_VRM_DIST		2
#define AQ5_FAN_VRM_UNDEF		0x7fff
#define AQ5_FLOW_OFFS			0x0fb
#define AQ5_FLOW_DIST			2
#define AQ5_CPU_TEMP_OFFS		0x0d7
#define AQ5_CPU_TEMP_DIST		2
#define AQ5_LEVEL_OFFS			0x147
#define AQ5_LEVEL_DIST			2

/* settings from HID feature report 0xB */
#define AQ5_SETTINGS_LEN			2428
#define AQ5_SETTINGS_FAN_OFFS			0x20d
#define AQ5_SETTINGS_FAN_DIST			20
#define AQ5_SETTINGS_TEMP_OFFS_OFFS		0x0dc
#define AQ5_SETTINGS_VRM_TEMP_OFFS_OFFS		0x134
#define AQ5_SETTINGS_CPU_TEMP_OFFS_OFFS		0x14c
#define AQ5_SETTINGS_LANGUAGE_OFFS		0x01a
#define AQ5_SETTINGS_TEMP_UNITS_OFFS		0x0c5
#define AQ5_SETTINGS_FLOW_UNITS_OFFS		0x0c6
#define AQ5_SETTINGS_PRESSURE_UNITS_OFFS	0x0c7
#define AQ5_SETTINGS_DECIMAL_SEPARATOR_OFFS	0x0c8
#define AQ5_SETTINGS_INFO_PAGE_OFFS		0x031
#define AQ5_SETTINGS_INFO_PAGE_DIST		4
#define AQ5_SETTINGS_KEY_DOWN_SENS_OFFS		0x009
#define AQ5_SETTINGS_KEY_ENTER_SENS_OFFS	0x00b
#define AQ5_SETTINGS_KEY_UP_SENS_OFFS		0x00d
#define AQ5_SETTINGS_KEY_PROG4_SENS_OFFS	0x011
#define AQ5_SETTINGS_KEY_PROG3_SENS_OFFS	0x013
#define AQ5_SETTINGS_KEY_PROG2_SENS_OFFS	0x015
#define AQ5_SETTINGS_KEY_PROG1_SENS_OFFS	0x017
#define AQ5_SETTINGS_DISABLE_KEYS_OFFS		0x00f
#define AQ5_SETTINGS_BRT_WHILE_IN_USE_OFFS	0x027
#define AQ5_SETTINGS_BRT_WHILE_IDLE_OFFS	0x029
#define AQ5_SETTINGS_ILLUM_MODE_OFFS		0x02b
#define AQ5_SETTINGS_KEY_TONE_OFFS		0x0c9
#define AQ5_SETTINGS_DISP_CONTRAST_OFFS		0x01b
#define AQ5_SETTINGS_DISP_BRT_WHILE_IN_USE_OFFS	0x01f
#define AQ5_SETTINGS_DISP_BRT_WHILE_IDLE_OFFS	0x021
#define AQ5_SETTINGS_DISP_ILLUM_TIME_OFFS	0x023
#define AQ5_SETTINGS_DISP_ILLUM_MODE_OFFS	0x025
#define AQ5_SETTINGS_DISP_MODE_OFFS		0x026
#define AQ5_SETTINGS_MENU_DISP_DURATION_OFFS	0x02c
#define AQ5_SETTINGS_DISP_DURATION_APS_OFFS	0x0d4
#define AQ5_SETTINGS_TIME_ZONE_OFFS		0x02f
#define AQ5_SETTINGS_DATE_SETTINGS_OFFS		0x030	
#define AQ5_SETTINGS_STNDBY_DISP_CONTRAST_OFFS	0x01d
#define AQ5_SETTINGS_STNDBY_LCD_BL_BRT_OFFS	0x0ca
#define AQ5_SETTINGS_STNDBY_KEY_BL_BRT_OFFS	0x0ce
#define AQ5_SETTINGS_STNDBY_TMO_KAD_BRT_OFFS	0x0cc
#define AQ5_SETTINGS_STNDBY_ACT_PWR_DOWN_OFFS	0x0d2
#define AQ5_SETTINGS_STNDBY_ACT_PWR_ON_OFFS	0x0d0

/* Fan settings control mode masks */
#define AQ5_SETTINGS_CTRL_MODE_REG_MODE_OUTPUT	0x0000	
#define AQ5_SETTINGS_CTRL_MODE_REG_MODE_RPM	0x0001
#define AQ5_SETTINGS_CTRL_MODE_PROG_FUSE	0x0200
#define AQ5_SETTINGS_CTRL_MODE_STARTBOOST	0x0400
#define AQ5_SETTINGS_CTRL_MODE_HOLD_MIN_POWER	0x0100


/* device-specific globals */
/* TODO: vectorize to handle more than one device */
unsigned char aq5_buf_data[AQ5_DATA_LEN];
unsigned char aq5_buf_settings[AQ5_SETTINGS_LEN];
int aq5_fd = -1;

typedef struct {
	int	val;
	char	*str;
} val_str_t;

/* Language setting strings */
val_str_t language_strings[] = {
	{ (language_t)ENGLISH,	"English" },
	{ (language_t)GERMAN,	"German" },
	{ -1,			"Unknown language"}
};

/* Temperature units strings */
val_str_t temp_units_strings[] = {
	{ (temp_units_t)CELSIUS,	"Celsius" },
	{ (temp_units_t)FAHRENHEIT,	"Fahrenheit" },
	{ (temp_units_t)KELVIN,		"Kelvin" },
	{ -1,				"Unknown temperature units"}
};

/* Flow units strings */
val_str_t flow_units_strings[] = {
	{ (flow_units_t)LPH,		"Liters per hour" },
	{ (flow_units_t)LPM,		"Liters per minute" },
	{ (flow_units_t)GPH_US,		"Gallons per hour (US)" },
	{ (flow_units_t)GPM_US,		"Gallons per minute (US)" },
	{ (flow_units_t)GPH_IMP,	"Gallons per hour (Imp)" },
	{ (flow_units_t)GPM_IMP,	"Gallons per minute (Imp)" },
	{ -1,				"Unknown flow units"}
};

/* Pressure units strings */
val_str_t pressure_units_strings[] = {
	{ (pressure_units_t)BAR,	"bar" },
	{ (pressure_units_t)PSI,	"PSI" },
	{ -1,				"Unknown pressure units"}
};

/* Decimal separator strings */
val_str_t decimal_separator_strings[] = {
	{ (decimal_separator_t)POINT,	"point" },
	{ (decimal_separator_t)COMMA,	"comma" },
	{ -1,				"Unknown decimal separator"}
};

/* Fan data source strings */
val_str_t fan_data_source_strings[] = {
	{ (fan_data_source_t)NONE,		"No data source" },
	{ (fan_data_source_t)TARGET_VAL_CONT_1,	"Target value controller 1" },
	{ (fan_data_source_t)TARGET_VAL_CONT_2,	"Target value controller 2" },
	{ (fan_data_source_t)TARGET_VAL_CONT_3,	"Target value controller 3" },
	{ (fan_data_source_t)TARGET_VAL_CONT_4,	"Target value controller 4" },
	{ (fan_data_source_t)TARGET_VAL_CONT_5,	"Target value controller 5" },
	{ (fan_data_source_t)TARGET_VAL_CONT_6,	"Target value controller 6" },
	{ (fan_data_source_t)TARGET_VAL_CONT_7,	"Target value controller 7" },
	{ (fan_data_source_t)TARGET_VAL_CONT_8,	"Target value controller 8" },
	{ (fan_data_source_t)TWO_POINT_CONT_1,	"Two point controller 1" },
	{ (fan_data_source_t)TWO_POINT_CONT_2,	"Two point controller 2" },
	{ (fan_data_source_t)TWO_POINT_CONT_3,	"Two point controller 3" },
	{ (fan_data_source_t)TWO_POINT_CONT_4,	"Two point controller 4" },
	{ (fan_data_source_t)TWO_POINT_CONT_5,	"Two point controller 5" },
	{ (fan_data_source_t)TWO_POINT_CONT_6,	"Two point controller 6" },
	{ (fan_data_source_t)TWO_POINT_CONT_7,	"Two point controller 7" },
	{ (fan_data_source_t)TWO_POINT_CONT_8,	"Two point controller 8" },
	{ (fan_data_source_t)TWO_POINT_CONT_9,	"Two point controller 9" },
	{ (fan_data_source_t)TWO_POINT_CONT_10,	"Two point controller 10" },
	{ (fan_data_source_t)TWO_POINT_CONT_11,	"Two point controller 11" },
	{ (fan_data_source_t)TWO_POINT_CONT_12,	"Two point controller 12" },
	{ (fan_data_source_t)TWO_POINT_CONT_13,	"Two point controller 13" },
	{ (fan_data_source_t)TWO_POINT_CONT_14,	"Two point controller 14" },
	{ (fan_data_source_t)TWO_POINT_CONT_15,	"Two point controller 15" },
	{ (fan_data_source_t)TWO_POINT_CONT_16,	"Two point controller 16" },
	{ (fan_data_source_t)CURVE_CTRLR_1,	"Curve controller 1" },
	{ (fan_data_source_t)CURVE_CTRLR_2,	"Curve controller 2" },
	{ (fan_data_source_t)CURVE_CTRLR_3,	"Curve controller 3" },
	{ (fan_data_source_t)CURVE_CTRLR_4,	"Curve controller 4" },
	{ (fan_data_source_t)RGB_LED_RED,	"RGB LED red" },
	{ (fan_data_source_t)RGB_LED_BLUE,	"RGB LED blue" },
	{ (fan_data_source_t)RGB_LED_GREEN,	"RGB LED green" },
	{ (fan_data_source_t)PRESET_VAL_1,	"Preset value 1" },
	{ (fan_data_source_t)PRESET_VAL_2,	"Preset value 2" },
	{ (fan_data_source_t)PRESET_VAL_3,	"Preset value 3" },
	{ (fan_data_source_t)PRESET_VAL_4,	"Preset value 4" },
	{ (fan_data_source_t)PRESET_VAL_5,	"Preset value 5" },
	{ (fan_data_source_t)PRESET_VAL_6,	"Preset value 6" },
	{ (fan_data_source_t)PRESET_VAL_7,	"Preset value 7" },
	{ (fan_data_source_t)PRESET_VAL_8,	"Preset value 8" },
	{ (fan_data_source_t)PRESET_VAL_9,	"Preset value 9" },
	{ (fan_data_source_t)PRESET_VAL_10,	"Preset value 10" },
	{ (fan_data_source_t)PRESET_VAL_11,	"Preset value 11" },
	{ (fan_data_source_t)PRESET_VAL_12,	"Preset value 12" },
	{ (fan_data_source_t)PRESET_VAL_13,	"Preset value 13" },
	{ (fan_data_source_t)PRESET_VAL_14,	"Preset value 14" },
	{ (fan_data_source_t)PRESET_VAL_15,	"Preset value 15" },
	{ (fan_data_source_t)PRESET_VAL_16,	"Preset value 16" },
	{ (fan_data_source_t)PRESET_VAL_17,	"Preset value 17" },
	{ (fan_data_source_t)PRESET_VAL_18,	"Preset value 18" },
	{ (fan_data_source_t)PRESET_VAL_19,	"Preset value 19" },
	{ (fan_data_source_t)PRESET_VAL_20,	"Preset value 20" },
	{ (fan_data_source_t)PRESET_VAL_21,	"Preset value 21" },
	{ (fan_data_source_t)PRESET_VAL_22,	"Preset value 22" },
	{ (fan_data_source_t)PRESET_VAL_23,	"Preset value 23" },
	{ (fan_data_source_t)PRESET_VAL_24,	"Preset value 24" },
	{ (fan_data_source_t)PRESET_VAL_25,	"Preset value 25" },
	{ (fan_data_source_t)PRESET_VAL_26,	"Preset value 26" },
	{ (fan_data_source_t)PRESET_VAL_27,	"Preset value 27" },
	{ (fan_data_source_t)PRESET_VAL_28,	"Preset value 28" },
	{ (fan_data_source_t)PRESET_VAL_29,	"Preset value 29" },
	{ (fan_data_source_t)PRESET_VAL_30,	"Preset value 30" },
	{ (fan_data_source_t)PRESET_VAL_31,	"Preset value 31" },
	{ (fan_data_source_t)PRESET_VAL_32,	"Preset value 32" },
	{ -1,					"Unknown data source" }
};

/* Info page strings */
val_str_t info_page_strings[] = {
	{ (info_page_type_t)LOG_DATA_1,		"Log data 1" },
	{ (info_page_type_t)LOG_DATA_2,		"Log data 2" },
	{ (info_page_type_t)LOG_DATA_3,		"Log data 3" },
	{ (info_page_type_t)LOG_DATA_4,		"Log data 4" },
	{ (info_page_type_t)LOG_DATA_5,		"Log data 5" },
	{ (info_page_type_t)LOG_DATA_6,		"Log data 6" },
	{ (info_page_type_t)LOG_DATA_7,		"Log data 7" },
	{ (info_page_type_t)LOG_DATA_8,		"Log data 8" },
	{ (info_page_type_t)LOG_DATA_9,		"Log data 9" },
	{ (info_page_type_t)LOG_DATA_10,	"Log data 10" },
	{ (info_page_type_t)LOG_DATA_11,	"Log data 11" },
	{ (info_page_type_t)LOG_DATA_12,	"Log data 12" },
	{ (info_page_type_t)LOG_DATA_13,	"Log data 13" },
	{ (info_page_type_t)LOG_DATA_14,	"Log data 14" },
	{ (info_page_type_t)LOG_DATA_15,	"Log data 15" },
	{ (info_page_type_t)LOG_DATA_16,	"Log data 16" },
	{ (info_page_type_t)SENSOR_1_2,		"Sensor 1+2" },
	{ (info_page_type_t)SENSOR_3_4,		"Sensor 3+4" },
	{ (info_page_type_t)SENSOR_5_6,		"Sensor 5+6" },
	{ (info_page_type_t)SENSOR_7_8,		"Sensor 7+8" },
	{ (info_page_type_t)POWERADJUST_1_2,	"Poweradjust 1+2" },
	{ (info_page_type_t)POWERADJUST_3_4,	"Poweradjust 3+4" },
	{ (info_page_type_t)POWERADJUST_5_6,	"Poweradjust 5+6" },
	{ (info_page_type_t)POWERADJUST_7_8,	"Poweradjust 7+8" },
	{ (info_page_type_t)SOFT_SENSOR_1_2,	"Software sensor 1+2" },
	{ (info_page_type_t)SOFT_SENSOR_3_4,	"Software sensor 3+4" },
	{ (info_page_type_t)SOFT_SENSOR_5_6,	"Software sensor 5+6" },
	{ (info_page_type_t)SOFT_SENSOR_7_8,	"Software sensor 7+8" },
	{ (info_page_type_t)VIRT_SENSOR_1_2,	"Virtual sensor 1+2" },
	{ (info_page_type_t)VIRT_SENSOR_3_4,	"Virtual sensor 3+4" },
	{ (info_page_type_t)MPS_1,		"MPS 1" },
	{ (info_page_type_t)MPS_2,		"MPS 2" },
	{ (info_page_type_t)MPS_3,		"MPS 3" },
	{ (info_page_type_t)MPS_4,		"MPS 4" },
	{ (info_page_type_t)AQUASTREAM_XT,	"Aquastream XT" },
	{ (info_page_type_t)SENSOR_39_40,	"Sensor 39+40" },
	{ (info_page_type_t)SENSOR_1_4,		"Sensor 1-4" },
	{ (info_page_type_t)SENSOR_5_8,		"Sensor 5-8" },
	{ (info_page_type_t)POWERADJUST_1_4,	"Poweradjust 1-4" },
	{ (info_page_type_t)POWERADJUST_5_8,	"Poweradjust 5-8" },
	{ (info_page_type_t)SOFT_SENSOR_1_4,	"Software sensor 1-4" },
	{ (info_page_type_t)SOFT_SENSOR_5_8,	"Software sensor 5-8" },
	{ (info_page_type_t)VIRT_SENSORS,	"Virtual sensors" },
	{ (info_page_type_t)MPS_1_2,		"MPS 1+2" },
	{ (info_page_type_t)MPS_3_4,		"MPS 3+4" },
	{ (info_page_type_t)AQUASTREAM,		"Aquastream" },
	{ (info_page_type_t)SENSOR_41_44,	"Sensor 41-44" },
	{ (info_page_type_t)FAN_AMP_1_4,	"Fan AMP 1-4" },
	{ (info_page_type_t)FAN_AMP_5_8,	"Fan AMP 5-8" },
	{ (info_page_type_t)FAN_AMP_9_12,	"Fan AMP 9-12" },
	{ (info_page_type_t)SENSOR_56_60,	"Sensor 56-60" },
	{ (info_page_type_t)SENSOR_61_64,	"Sensor 61-64" },
	{ (info_page_type_t)FAN_1_4_POWER,	"Fan 1-4 power" },
	{ (info_page_type_t)FAN_5_8_POWER,	"Fan 5-8 power" },
	{ (info_page_type_t)FAN_9_12_POWER,	"Fan 9-12 power" },
	{ (info_page_type_t)FAN_1_4_RPM,	"Fan 1-4 RPM" },
	{ (info_page_type_t)FAN_5_8_RPM,	"Fan 5-8 RPM" },
	{ (info_page_type_t)FAN_9_12_RPM,	"Fan 9-12 RPM" },
	{ (info_page_type_t)FAN_1,		"Fan 1" },
	{ (info_page_type_t)FAN_2,		"Fan 2" },
	{ (info_page_type_t)FAN_3,		"Fan 3" },
	{ (info_page_type_t)FAN_4,		"Fan 4" },
	{ (info_page_type_t)FAN_5,		"Fan 5" },
	{ (info_page_type_t)FAN_6,		"Fan 6" },
	{ (info_page_type_t)FAN_7,		"Fan 7" },
	{ (info_page_type_t)FAN_8,		"Fan 8" },
	{ (info_page_type_t)FAN_9,		"Fan 9" },
	{ (info_page_type_t)FAN_10,		"Fan 10" },
	{ (info_page_type_t)FAN_11,		"Fan 11" },
	{ (info_page_type_t)FAN_12,		"Fan 12" },
	{ (info_page_type_t)FLOW_1,		"Flow 1" },
	{ (info_page_type_t)FLOW_2,		"Flow 2" },
	{ (info_page_type_t)FLOW_3,		"Flow 3" },
	{ (info_page_type_t)FLOW_4,		"Flow 4" },
	{ (info_page_type_t)FLOW_5,		"Flow 5" },
	{ (info_page_type_t)FLOW_6,		"Flow 6" },
	{ (info_page_type_t)FLOW_7,		"Flow 7" },
	{ (info_page_type_t)FLOW_8,		"Flow 8" },
	{ (info_page_type_t)FLOW_9,		"Flow 9" },
	{ (info_page_type_t)FLOW_10,		"Flow 10" },
	{ (info_page_type_t)FLOW_11,		"Flow 11" },
	{ (info_page_type_t)FLOW_12,		"Flow 12" },
	{ (info_page_type_t)FLOW_13,		"Flow 13" },
	{ (info_page_type_t)FLOW_14,		"Flow 14" },
	{ (info_page_type_t)AQUASTREAM_XT_1,	"Aquastream XT 1" },
	{ (info_page_type_t)AQUASTREAM_XT_2,	"Aquastream XT 2" },
	{ (info_page_type_t)MULTISWITCH_1,	"Multiswitch 1" },
	{ (info_page_type_t)MULTISWITCH_2,	"Multiswitch 2" },
	{ (info_page_type_t)FILL_LEVEL_1,	"Fill level 1" },
	{ (info_page_type_t)FILL_LEVEL_2,	"Fill level 2" },
	{ (info_page_type_t)FILL_LEVEL_3,	"Fill level 3" },
	{ (info_page_type_t)FILL_LEVEL_4,	"Fill level 4" },
	{ (info_page_type_t)PWR_CONSUMPT_1,	"Power consumption 1" },
	{ (info_page_type_t)PWR_CONSUMPT_2,	"Power consumption 2" },
	{ (info_page_type_t)PWR_CONSUMPT_3,	"Power consumption 3" },
	{ (info_page_type_t)PWR_CONSUMPT_4,	"Power consumption 4" },
	{ (info_page_type_t)AQUAERO_INFO,	"Aquaero info" },
	{ (info_page_type_t)TIME,		"Time" },
	{ (info_page_type_t)USB_LCD_PAGE,	"USB LCD page" },
	{ (info_page_type_t)RELAY_AND_POWER,	"Relay and power outputs" },
	{ (info_page_type_t)USER_DEF_LOGO,	"User defined logo" },
	{ -1,					"Unknown info page"}
};

/* Info page display mode strings */
val_str_t page_display_mode_strings[] = {
	{ (page_display_mode_t)HIDE_PAGE,	"Hide page" },
	{ (page_display_mode_t)SHOW_PAGE,	"Show page" },
	{ (page_display_mode_t)SHOW_PAGE_PERM,	"Show page permanently" },
	{ -1,					"Unknown display mode" }
};

/* disable keys strings */
val_str_t disable_keys_strings[] = {
	{ (disable_keys_t)ENABLED,	"Enabled" },
	{ (disable_keys_t)DISABLED,	"Disabled" },
	{ -1,				"Unknown disable keys state"}
};

/* Illumination mode strings */
val_str_t illumination_mode_strings[] = {
	{ (illumination_mode_t)AUTO_OFF,	"Automatic off" },
	{ (illumination_mode_t)ALWAYS_ON,	"Always on" },
	{ -1,					"Unknown illumination mode"}
};

/* Key tone strings */
val_str_t key_tone_strings[] = {
	{ (key_tone_t)OFF,	"Off" },
	{ (key_tone_t)QUIET,	"Quiet" },
	{ (key_tone_t)NORMAL,	"Normal" },
	{ (key_tone_t)LOUD,	"Loud" },
	{ -1,			"Unknown key tone mode" }
};

/* Date formt strings */
val_str_t date_format_strings[] = {
	{ (date_format_t)YEAR_MONTH_DAY,	"Year.Month.Day" },
	{ (date_format_t)DAY_MONTH_YEAR,	"Day.Month.Year" },
	{ -1,					"Unknown date format"}
};

/* Time format strings */
val_str_t time_format_strings[] = {
	{ (time_format_t)TWELVE_HOUR,		"12 hour" },
	{ (time_format_t)TWENTY_FOUR_HOUR,	"24 hour" },
	{ -1,					"Unknown time format" }
};

/* Auto DST format strings */
val_str_t auto_dst_strings[] = {
	{ (auto_dst_t)DISABLED,	"Enabled" },
	{ (auto_dst_t)ENABLED,	"Disabled" },
	{ -1,			"Unknown DST mode" },
	
};

/* Display mode strings */
val_str_t display_mode_strings[] = {
	{ (display_mode_t)DISP_NORMAL,		"Normal" },
	{ (display_mode_t)DISP_INVERTED,	"Inverted" },
	{ -1,					"Unknown display mode"}
};

/* Standby action strings */
val_str_t standby_action_strings[] = {
	{ (standby_action_t)NO_ACTION,			"No action" },
	{ (standby_action_t)SPEED_SIG_GEN_ON,		"Speed signal generator on" },
	{ (standby_action_t)SPEED_SIG_GEN_OFF,		"Speed signal generator off" },
	{ (standby_action_t)ALARM_BUZZER_ON,		"Alarm buzzer on" },
	{ (standby_action_t)ALARM_BUZZER_OFF,		"Alarm buzzer off" },
	{ (standby_action_t)ALARM_BUZZER_CYCLE_ON_OFF,	"Alarm buzzer cycle on-off" },
	{ (standby_action_t)ALARM_BUZZER_SINGLE_TONE,	"Alarm buzzer single tone" },
	{ (standby_action_t)RELAY_ON,			"Relay on" },
	{ (standby_action_t)RELAY_OFF,			"Relay off" },
	{ (standby_action_t)SWITCH_RELAY_2_SEC,		"Switch relay for 2 s" },
	{ (standby_action_t)SWITCH_RELAY_10_SEC,	"Switch relay for 10 s" },
	{ (standby_action_t)LOAD_PROFILE_1,		"Load profile 1" },
	{ (standby_action_t)LOAD_PROFILE_2,		"Load profile 2" },
	{ (standby_action_t)LOAD_PROFILE_3,		"Load profile 3" },
	{ (standby_action_t)LOAD_PROFILE_4,		"Load profile 4" },
	{ (standby_action_t)USB_KEYBD_POWER_KEY,	"USB keyboard: power key" },
	{ (standby_action_t)USB_KEYBD_SLEEP_KEY,	"USB keyboard: sleep key" },
	{ (standby_action_t)USB_KEYBD_WAKEUP_KEY,	"USB keyboard: wakeup key" },
	{ -1,						"Unknown standby action" },
};

/* helper functions */

inline uint16_t aq5_get_int16(unsigned char *buffer, short offset)
{
	return (uint16_t)((buffer[offset] << 8) | buffer[offset + 1]);
}


inline uint32_t aq5_get_int32(unsigned char *buffer, short offset)
{
	return (buffer[offset] << 24) | (buffer[offset + 1] << 16) |
			(buffer[offset + 2] << 8) | buffer[offset + 3];
}


/* get the uptime for the given value in seconds */
inline void aq5_get_uptime(uint32_t timeval, aq5_time_t *uptime)
{
	uptime->tm_sec = timeval;
	uptime->tm_min = uptime->tm_sec / 60;
	uptime->tm_hour = uptime->tm_min / 60;
	uptime->tm_mday = uptime->tm_hour / 24;
	if (uptime->tm_sec > 59)
		uptime->tm_sec -= 60 * uptime->tm_min;
	if (uptime->tm_min > 59)
		uptime->tm_min -= 60 * uptime->tm_hour;
	if (uptime->tm_hour > 23)
		uptime->tm_hour -= 24 * uptime->tm_mday;
}


inline void aq5_get_time(uint32_t timeval, aq5_time_t *time)
{
	time->tm_min = 0;
	time->tm_hour = 0;
	time->tm_mday = 1;
	time->tm_mon = 0;
	time->tm_year = 109;
	time->tm_gmtoff = 0;
	time->tm_sec = timeval;
	mktime(time);
}


char *aq5_strcat(char *str1, char *str2)
{
	char *ret;

	if ((ret = malloc(strlen(str1) + strlen(str2) + 1)) == NULL)
		return NULL;
	strcpy(ret, str1);
	strcpy(ret + strlen(str1), str2);

	return ret;
}

int aq5_open(char *device, char **err_msg)
{
	struct hiddev_devinfo dinfo;

	/* Only open the device if we need to */
	if (fcntl(aq5_fd, F_GETFL) != -1)
		/* The file handle is already defined and open, just continue */
		return 0;

	/* no device given, search */
	if (device == NULL) {
		DIR *dp;
		struct dirent *ep;
		char *full_path = NULL;

		if ((dp = opendir("/dev/usb")) == NULL) {
			*err_msg = "failed to open directory '/dev/usb'";
			return -1;
		}

		while ((ep = readdir(dp)) != NULL) {
			/* search for files beginning with 'hiddev' */
			if (strncmp(ep->d_name, "hiddev", 6) != 0) {
				continue;
			}
			full_path = aq5_strcat("/dev/usb/", ep->d_name);
			if ((aq5_fd = open(full_path, O_RDONLY)) < 0) {
#ifdef DEBUG
				printf("failed to open '%s', skipping\n", full_path);
#endif
				continue;
			}
			ioctl(aq5_fd, HIDIOCGDEVINFO, &dinfo);
			if ((dinfo.vendor != AQ5_USB_VID) || ((dinfo.product & 0xffff) !=
					AQ5_USB_PID)) {
#ifdef DEBUG
				printf("no Aquaero 5 found on '%s'\n", full_path);
#endif
				close(aq5_fd);
				continue;
			} else {
				/* we found the first Aquaero 5 device */
#ifdef DEBUG
				printf("found Aquaero 5 device on '%s'!\n", full_path);
#endif
				break;
			}
		}
		closedir(dp);

		if (fcntl(aq5_fd, F_GETFL) == -1) {
			*err_msg = "failed to find an Aquaero 5 device";
			return -1;
		}

		return 0;
	}


	/* device name given, try using it */
	aq5_fd = open(device, O_RDONLY);
	if (fcntl(aq5_fd, F_GETFL) == -1) {
		/* We tried to open the device but failed */
		*err_msg = "failed to open device";
		return -1;
	}

#ifdef DEBUG
	u_int32_t version;
	ioctl(aq5_fd, HIDIOCGVERSION, &version);
	printf("hiddev driver version is %d.%d.%d\n", version >> 16,
			(version >> 8) & 0xff, version & 0xff);
#endif

	ioctl(aq5_fd, HIDIOCGDEVINFO, &dinfo);
	if ((dinfo.vendor != AQ5_USB_VID) ||
			((dinfo.product & 0xffff) != AQ5_USB_PID)) {
		printf("No Aquaero 5 found on %s. Found vendor:0x%x, product:0x%x(0x%x), version 0x%x instead\n",
				device, dinfo.vendor, dinfo.product & 0xffff, dinfo.product,
				dinfo.version);
		close(aq5_fd);
		return -1;
	}

#ifdef DEBUG
	struct hiddev_string_descriptor hStr;
	hStr.index = 2; /* Vendor = 1, Product = 2 */
	hStr.value[0] = 0;
	ioctl(aq5_fd, HIDIOCGSTRING, &hStr);
	printf("Found '%s'\n", hStr.value);
#endif

	return 0;
}


/* Get the specified HID report */
int aq5_get_report(int fd, int report_id, unsigned report_type, unsigned char *report_data)
{
	struct hiddev_report_info rinfo;
	struct hiddev_field_info finfo;
	struct hiddev_usage_ref uref;
	int j;

	rinfo.report_type = report_type;
	rinfo.report_id = report_id;
	if (ioctl(fd, HIDIOCGREPORTINFO, &rinfo) != 0) {
		fprintf(stderr, "failed to fetch input report id %d\n", report_id);
		return -1;
	}
	/* printf("HIDIOCGREPORTINFO: report_id=0x%X (%u fields)\n", rinfo.report_id, rinfo.num_fields); */
	finfo.report_type = rinfo.report_type;
	finfo.report_id   = rinfo.report_id;
	finfo.field_index = 0; /* There is only one field for the Aquaero reports */
	if (ioctl(fd, HIDIOCGFIELDINFO, &finfo) != 0) {
		fprintf(stderr, "failed to fetch field info, report %d\n", report_id);
		return -1;
	}

	/* Put the report ID into the first byte to be consistant with hidraw */
	report_data[0] = report_id;
	/* printf("Max usage is %d\n", finfo.maxusage); */
	for (j = 0; j < finfo.maxusage; j++) {
		uref.report_type = finfo.report_type;
		uref.report_id   = finfo.report_id;
		uref.field_index = 0;
		uref.usage_index = j;
		/* fetch the usage code for given indexes */
		ioctl(fd, HIDIOCGUCODE, &uref);
		/* fetch the value from report */
		ioctl(fd, HIDIOCGUSAGE, &uref);
		report_data[j+1] = uref.value;
	}

	return 0;
}


/* Close the file handle and shut down */
void libaquaero5_exit()
{
	close(aq5_fd);
}


/* Get the settings feature report ID 0xB (11) */
int libaquaero5_getsettings(char *device, aq5_settings_t *settings_dest, char **err_msg)
{
	int res;

	/* Allow the device to be disconnected and open only if the fd is undefined */
	if (aq5_open(device, err_msg) != 0) {
		return -1;
	}

	res = aq5_get_report(aq5_fd, 0xB, HID_REPORT_TYPE_FEATURE, aq5_buf_settings);
	if (res != 0) {
		libaquaero5_exit();
		printf("failed to get report!");
		return -1;	
	}

	/* User interface settings */
	settings_dest->disable_keys = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_DISABLE_KEYS_OFFS);
	settings_dest->brightness_while_in_use = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_BRT_WHILE_IN_USE_OFFS);
	settings_dest->brightness_while_idle = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_BRT_WHILE_IDLE_OFFS);
	settings_dest->illumination_mode = aq5_buf_settings[AQ5_SETTINGS_ILLUM_MODE_OFFS];
	settings_dest->key_tone = aq5_buf_settings[AQ5_SETTINGS_KEY_TONE_OFFS];
	settings_dest->key_sensitivity.down_key = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_KEY_DOWN_SENS_OFFS);	
	settings_dest->key_sensitivity.enter_key = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_KEY_ENTER_SENS_OFFS);	
	settings_dest->key_sensitivity.up_key = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_KEY_UP_SENS_OFFS);	
	settings_dest->key_sensitivity.programmable_key_4 = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_KEY_PROG4_SENS_OFFS);	
	settings_dest->key_sensitivity.programmable_key_3 = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_KEY_PROG3_SENS_OFFS);	
	settings_dest->key_sensitivity.programmable_key_2 = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_KEY_PROG2_SENS_OFFS);	
	settings_dest->key_sensitivity.programmable_key_1 = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_KEY_PROG1_SENS_OFFS);	

	settings_dest->language = aq5_buf_settings[AQ5_SETTINGS_LANGUAGE_OFFS];

	settings_dest->time_zone = aq5_buf_settings[AQ5_SETTINGS_TIME_ZONE_OFFS];
	settings_dest->display_contrast = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_DISP_CONTRAST_OFFS);
	settings_dest->display_brightness_while_in_use = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_DISP_BRT_WHILE_IN_USE_OFFS);
	settings_dest->display_brightness_while_idle = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_DISP_BRT_WHILE_IDLE_OFFS);
	settings_dest->display_illumination_time = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_DISP_ILLUM_TIME_OFFS);
	settings_dest->display_illumination_mode = aq5_buf_settings[AQ5_SETTINGS_DISP_ILLUM_MODE_OFFS];
	settings_dest->display_mode = aq5_buf_settings[AQ5_SETTINGS_DISP_MODE_OFFS];
	settings_dest->menu_display_duration = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_MENU_DISP_DURATION_OFFS);
	settings_dest->display_duration_after_page_selection = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_DISP_DURATION_APS_OFFS);

	for (int i=0; i<AQ5_NUM_INFO_PAGE; i++) {
		settings_dest->info_page[i].page_display_mode = aq5_buf_settings[AQ5_SETTINGS_INFO_PAGE_OFFS + i * AQ5_SETTINGS_INFO_PAGE_DIST];
		settings_dest->info_page[i].display_time = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_INFO_PAGE_OFFS + 1 + i * AQ5_SETTINGS_INFO_PAGE_DIST);
		settings_dest->info_page[i].info_page_type = aq5_buf_settings[AQ5_SETTINGS_INFO_PAGE_OFFS + 3 + i * AQ5_SETTINGS_INFO_PAGE_DIST];
	}

	settings_dest->temp_units = aq5_buf_settings[AQ5_SETTINGS_TEMP_UNITS_OFFS];
	settings_dest->flow_units = aq5_buf_settings[AQ5_SETTINGS_FLOW_UNITS_OFFS];
	settings_dest->pressure_units = aq5_buf_settings[AQ5_SETTINGS_PRESSURE_UNITS_OFFS];
	settings_dest->decimal_separator = aq5_buf_settings[AQ5_SETTINGS_DECIMAL_SEPARATOR_OFFS];

	/* System settings */
	int m;
	settings_dest->time_zone = aq5_buf_settings[AQ5_SETTINGS_TIME_ZONE_OFFS];

	m = aq5_buf_settings[AQ5_SETTINGS_DATE_SETTINGS_OFFS];
	if ((m & (date_format_t)DAY_MONTH_YEAR) == (date_format_t)DAY_MONTH_YEAR) {
		settings_dest->date_format = DAY_MONTH_YEAR;
	} else {
		settings_dest->date_format = YEAR_MONTH_DAY;
	}
	
	if ((m & (time_format_t)TWENTY_FOUR_HOUR) == (time_format_t)TWENTY_FOUR_HOUR) {
		settings_dest->time_format = TWENTY_FOUR_HOUR;
	} else {
		settings_dest->time_format = TWELVE_HOUR;
	}

	if ((m & (auto_dst_t)DST_ENABLED) == (auto_dst_t)DST_ENABLED) {
		settings_dest->auto_dst = DST_ENABLED;
	} else {
		settings_dest->auto_dst = DST_DISABLED;
	}

	/* System - standby config */
	settings_dest->standby_display_contrast = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_STNDBY_DISP_CONTRAST_OFFS);
	settings_dest->standby_lcd_backlight_brightness = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_STNDBY_LCD_BL_BRT_OFFS);
	settings_dest->standby_key_backlight_brightness = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_STNDBY_KEY_BL_BRT_OFFS);
	settings_dest->standby_timeout_key_and_display_brightness = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_STNDBY_TMO_KAD_BRT_OFFS);
	settings_dest->standby_action_at_power_down = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_STNDBY_ACT_PWR_DOWN_OFFS);
	settings_dest->standby_action_at_power_up = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_STNDBY_ACT_PWR_ON_OFFS);

	/* CPU temperature offset setting */
	for (int i=0; i<AQ5_NUM_TEMP; i++) {
		settings_dest->temp_offset[i] = (double)aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_TEMP_OFFS_OFFS + i * AQ5_TEMP_DIST) /100.0;
	}
	
	/* Fan VRM temperature offset setting */
	for (int i=0; i<AQ5_NUM_FAN; i++) {
		settings_dest->fan_vrm_temp_offset[i] = (double)aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_VRM_TEMP_OFFS_OFFS + i * AQ5_TEMP_DIST) /100.0;
	}

	/* CPU temperature offset setting */
	for (int i=0; i<AQ5_NUM_CPU; i++) {
		settings_dest->cpu_temp_offset[i] = (double)aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_CPU_TEMP_OFFS_OFFS + i * AQ5_TEMP_DIST) /100.0;
	}

	/* fan settings */
	int n;
	for (int i=0; i<AQ5_NUM_FAN; i++) {
		settings_dest->fan_min_rpm[i] = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_FAN_OFFS + i * AQ5_SETTINGS_FAN_DIST);
		settings_dest->fan_max_rpm[i] = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_FAN_OFFS + 2 + i * AQ5_SETTINGS_FAN_DIST);
		settings_dest->fan_min_duty[i] = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_FAN_OFFS + 4 + i * AQ5_SETTINGS_FAN_DIST) / 100;
		settings_dest->fan_max_duty[i] = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_FAN_OFFS + 6 + i * AQ5_SETTINGS_FAN_DIST) / 100;
		settings_dest->fan_startboost_duty[i] = (double)aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_FAN_OFFS + 8 + i * AQ5_SETTINGS_FAN_DIST) / 100.0;
		settings_dest->fan_startboost_duration[i] = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_FAN_OFFS + 10 + i * AQ5_SETTINGS_FAN_DIST);
		settings_dest->fan_pulses_per_revolution[i] = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_FAN_OFFS + 12 + i * AQ5_SETTINGS_FAN_DIST);
		n = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_FAN_OFFS + 14 + i * AQ5_SETTINGS_FAN_DIST);
		settings_dest->fan_control_mode[i].fan_regulation_mode = n & AQ5_SETTINGS_CTRL_MODE_REG_MODE_RPM;
		if ((n & AQ5_SETTINGS_CTRL_MODE_PROG_FUSE) == AQ5_SETTINGS_CTRL_MODE_PROG_FUSE) {
			settings_dest->fan_control_mode[i].use_programmable_fuse = TRUE; 
		} else {
			settings_dest->fan_control_mode[i].use_programmable_fuse = FALSE;
		}
		if ((n & AQ5_SETTINGS_CTRL_MODE_STARTBOOST) == AQ5_SETTINGS_CTRL_MODE_STARTBOOST) {
			settings_dest->fan_control_mode[i].use_startboost = TRUE;
		} else {
			settings_dest->fan_control_mode[i].use_startboost = FALSE;
		}
		if ((n & AQ5_SETTINGS_CTRL_MODE_HOLD_MIN_POWER) == AQ5_SETTINGS_CTRL_MODE_HOLD_MIN_POWER) {
			settings_dest->fan_control_mode[i].hold_minimum_power = TRUE;
		} else {
			settings_dest->fan_control_mode[i].hold_minimum_power = FALSE;
		}
		settings_dest->fan_data_source[i] = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_FAN_OFFS + 16 + i * AQ5_SETTINGS_FAN_DIST);
		settings_dest->fan_programmable_fuse[i] = aq5_get_int16(aq5_buf_settings, AQ5_SETTINGS_FAN_OFFS + 18 + i * AQ5_SETTINGS_FAN_DIST);
	}

	return 0;
}


/* Get the current sensor data from input report 1 */
int libaquaero5_poll(char *device, aq5_data_t *data_dest, char **err_msg)
{
	int res;

	/* Allow the device to be disconnected and open only if the fd is undefined */
	if (aq5_open(device, err_msg) != 0) {
		return -1;
	}

	res = aq5_get_report(aq5_fd, 0x1, HID_REPORT_TYPE_INPUT, aq5_buf_data);
	if (res != 0) {
		libaquaero5_exit();
		printf("failed to get report!\n");
		return -1;	
	}

	/* current time */
	aq5_get_time(aq5_get_int32(aq5_buf_data, AQ5_CURRENT_TIME_OFFS), &data_dest->time_utc);

	/* device info */
	data_dest->serial_major = aq5_get_int16(aq5_buf_data, AQ5_SERIAL_MAJ_OFFS);
	data_dest->serial_minor = aq5_get_int16(aq5_buf_data, AQ5_SERIAL_MIN_OFFS);
	data_dest->firmware_version = aq5_get_int16(aq5_buf_data, AQ5_FIRMWARE_VER_OFFS);
	data_dest->bootloader_version = aq5_get_int16(aq5_buf_data, AQ5_BOOTLOADER_VER_OFFS);
	data_dest->hardware_version = aq5_get_int16(aq5_buf_data, AQ5_HARDWARE_VER_OFFS);

	/* operating times */
	aq5_get_uptime(aq5_get_int32(aq5_buf_data, AQ5_UPTIME_OFFS), &data_dest->uptime);
	aq5_get_uptime(aq5_get_int32(aq5_buf_data, AQ5_TOTAL_TIME_OFFS), &data_dest->total_time);

	/* temperature sensors */
	int n;
	for (int i=0; i<AQ5_NUM_TEMP; i++) {
		n = aq5_get_int16(aq5_buf_data, AQ5_TEMP_OFFS  + i * AQ5_TEMP_DIST);
		data_dest->temp[i] = n!=AQ5_TEMP_UNDEF ? (double)n/100.0 : AQ5_FLOAT_UNDEF;
	}

	/* fans */
	for (int i=0; i<AQ5_NUM_FAN; i++) {
		n = aq5_get_int16(aq5_buf_data, AQ5_FAN_VRM_OFFS + i * AQ5_FAN_VRM_DIST);
		data_dest->fan_vrm_temp[i] = n!=AQ5_FAN_VRM_UNDEF ? (double)n/100.0 : AQ5_FLOAT_UNDEF;
		data_dest->fan_rpm[i] = aq5_get_int16(aq5_buf_data, AQ5_FAN_OFFS + i * AQ5_FAN_DIST);
		data_dest->fan_duty[i] = aq5_get_int16(aq5_buf_data, AQ5_FAN_OFFS + 2 + i * AQ5_FAN_DIST) / 100;
		data_dest->fan_voltage[i] = (double)aq5_get_int16(aq5_buf_data, AQ5_FAN_OFFS + 4 + i * AQ5_FAN_DIST) / 100.0;
		data_dest->fan_current[i] = aq5_get_int16(aq5_buf_data, AQ5_FAN_OFFS + 6 + i * AQ5_FAN_DIST);
	}

	/* flow sensors */
	for (int i=0; i<AQ5_NUM_FLOW; i++) {
		data_dest->flow[i] = (double)aq5_get_int16(aq5_buf_data, AQ5_FLOW_OFFS + i * AQ5_FLOW_DIST) / 10.0;
	}

	/* CPU temp */
	for (int i=0; i<AQ5_NUM_CPU; i++) {
		n = (double)aq5_get_int16(aq5_buf_data, AQ5_CPU_TEMP_OFFS + i * AQ5_CPU_TEMP_DIST);
		data_dest->cpu_temp[i] = n!=AQ5_TEMP_UNDEF ? (double)n/100.0 : AQ5_FLOAT_UNDEF;
	}

	/* Liquid level sensors */
	for (int i=0; i<AQ5_NUM_LEVEL; i++) {
		data_dest->level[i] = (double)aq5_get_int16(aq5_buf_data, AQ5_LEVEL_OFFS + i * AQ5_LEVEL_DIST) / 100.0;
	}

	return 0;
}


int	libaquaero5_dump_data(char *file)
{
	FILE *outf = fopen(file, "w");
	if (outf == NULL)
		return -1;

	if (fwrite(aq5_buf_data, 1, AQ5_DATA_LEN, outf) != AQ5_DATA_LEN) {
		fclose(outf);
		return -1;
	}
	fclose(outf);

	return 0;
}


int	libaquaero5_dump_settings(char *file)
{
	FILE *outf = fopen(file, "w");
	if (outf == NULL)
		return -1;

	if (fwrite(aq5_buf_settings, 1, AQ5_SETTINGS_LEN, outf) !=
			AQ5_SETTINGS_LEN) {
		fclose(outf);
		return -1;
	}
	fclose(outf);

	return 0;
}

/* Return the human readable string for the given enum */
char *libaquaero5_get_string(int id, val_str_opt_t opt) 
{
	int i;
	val_str_t *val_str;
	switch (opt) {
		case STANDBY_ACTION:
			val_str = standby_action_strings;
			break;
		case DATE_FORMAT:
			val_str = date_format_strings;
			break;
		case TIME_FORMAT:
			val_str = time_format_strings;
			break;
		case AUTO_DST:
			val_str = auto_dst_strings;
			break;
		case DISPLAY_MODE:
			val_str = display_mode_strings;
			break;
		case FAN_DATA_SRC:
			val_str = fan_data_source_strings;
			break;
		case LANGUAGE:
			val_str = language_strings;
			break;
		case TEMP_UNITS:
			val_str = temp_units_strings;
			break;
		case FLOW_UNITS:
			val_str = flow_units_strings;
			break;
		case PRESSURE_UNITS:
			val_str = pressure_units_strings;
			break;
		case DECIMAL_SEPARATOR:
			val_str = decimal_separator_strings;
			break; 
		case INFO_SCREEN:
			val_str = info_page_strings;
			break;
		case PAGE_DISPLAY_MODE:
			val_str = page_display_mode_strings;
			break;
		case DISABLE_KEYS:
			val_str = disable_keys_strings;
			break;
		case ILLUM_MODE:
			val_str = illumination_mode_strings;
			break;
		case KEY_TONE:
		default:
			val_str = key_tone_strings;
	}
	/* We have to search for it */
	for (i=0; val_str[i].val != -1; i++) {
		if (id == val_str[i].val) {
			break;
		}
	}
	return (val_str[i].str);
}
