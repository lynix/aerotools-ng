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

#ifndef AQUAERO5_USER_STRINGS_H_
#define AQUAERO5_USER_STRINGS_H_

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


#endif /* AQUAERO5_USER_STRINGS_H_ */
