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

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>

#include "libaquaero5.h"

typedef enum { F_STD, F_SCRIPT } out_fmt_t;

char *device = NULL;
char *err_msg = NULL;
char *dump_data_file = NULL;
char *dump_settings_file = NULL;
out_fmt_t out_format = F_STD;
char out_all = 0;
char *temp_unit;
char *flow_unit;


void print_help()
{
	printf("Usage: aerocli [OPTIONS]\n\n");

	printf("Options:\n");
	printf("  -d  DEVICE  use given DEVICE (no auto-discovery)\n");
	printf("  -o  FORMAT  output format (default, export)\n");
	printf("  -a          print all available data (default: summary)\n");
	printf("  -D  FILE    dump data to FILE\n");
	printf("  -S  FILE    dump settings to FILE\n");
	printf("  -h          display this usage information\n");

	printf("\n");
	printf("This version of aerocli was built on %s %s.\n", __DATE__, __TIME__);
}


void parse_cmdline(int argc, char *argv[])
{
	char c;
	extern int optind, optopt, opterr;

	while ((c = getopt(argc, argv, "d:o:aD:S:h")) != -1) {
		switch (c) {
			case 'h':
				print_help();
				exit(EXIT_SUCCESS);
				break;
			case 'd':
				device = optarg;
				break;
			case 'D':
					dump_data_file = optarg;
					break;
			case 'S':
					dump_settings_file = optarg;
					break;
			case 'o':
				if (strcmp(optarg, "export") == 0) {
					out_format = F_SCRIPT;
				} else if (strcmp(optarg, "default") == 0) {
					out_format = F_STD;
				} else {
					fprintf(stderr, "invalid output format: '%s'\n", optarg);
					exit(EXIT_FAILURE);
				}
				break;
			case 'a':
				out_all = 1;
				break;
			case '?':
				if (optopt == 'd'|| optopt == 'o')
					fprintf(stderr, "option -%c requires an argument\n",
							optopt);
				exit(EXIT_FAILURE);
				break;
			default:
				fprintf(stderr, "invalid arguments. Try -h for help.");
				exit(EXIT_FAILURE);
		}
	}
}


inline void print_with_offset(double value, double offset, char *unit)
{
	printf("%5.2f %s (%+.2f)", value, unit, offset);
}


void print_system(aq5_data_t *aq_data, aq5_settings_t *aq_sett) {
	/* A klunky way to get the offset for the local time from UTC since mktime doesn't honor gmtoff */
	struct tm *local_aq_time, *systime;
	time_t aq_time_t, systime_t;
	aq_time_t = mktime(&aq_data->time_utc);
	time(&systime_t);
	systime = localtime(&systime_t);
	aq_time_t += systime->tm_gmtoff;
	local_aq_time = localtime(&aq_time_t);

	/* pre-convert times to strings */
	char time_local_str[21], uptime_str[51];
	strftime(time_local_str, 20, "%Y-%m-%d %H:%M:%S", local_aq_time);
	strftime(uptime_str, 50, "%dd %H:%M:%S", &aq_data->uptime);

	printf("----------- System -----------\n");
	if (!out_all) {
		printf("Time:%25s\n", time_local_str);
		printf("Uptime:%23s\n", uptime_str);
		printf("CPU Temp:%18.2f %s\n", aq_data->cpu_temp[0], temp_unit);
	} else {
		char time_utc_str[21], uptime_total_str[51];
		strftime(time_utc_str, 20, "%Y-%m-%d %H:%M:%S", &aq_data->time_utc);
		strftime(uptime_total_str, 50, "%yy %dd %H:%M:%S", &aq_data->total_time);
		printf("Time (UTC)    = %s\n", time_utc_str);
		printf("Time (local)  = %s\n", time_local_str);
		printf("Uptime        = %s\n", uptime_str);
		printf("Uptime total  = %s\n", uptime_total_str);
		printf("Serial number = %d-%d\n", aq_data->serial_major,
				aq_data->serial_minor);
		printf("Firmware      = %d\n", aq_data->firmware_version);
		printf("Bootloader    = %d\n", aq_data->bootloader_version);
		printf("Hardware      = %d\n", aq_data->hardware_version);
		printf("CPU Temp      = ");
		for (int n=0; n<AQ5_NUM_CPU; n++) {
			if (aq_data->cpu_temp[n] != AQ5_FLOAT_UNDEF)
				print_with_offset(aq_data->cpu_temp[n],
						aq_sett->cpu_temp_offset[n], temp_unit);
				putchar(' ');
		}
		putchar('\n');
	}
}


void print_sensors(aq5_data_t *aq_data, aq5_settings_t *aq_sett)
{
	printf("---- Temperature Sensors -----\n");
	if (!out_all) {
		for (int n=0; n<AQ5_NUM_TEMP; n++)
			if (aq_data->temp[n] != AQ5_FLOAT_UNDEF)
				printf("Sensor %2d:%17.2f %s\n", n+1, aq_data->temp[n], temp_unit);
		return;
	}

	for (int n=0; n<AQ5_NUM_TEMP; n++) {
		printf("Sensor %2d     = ", n+1);
		if (aq_data->temp[n] != AQ5_FLOAT_UNDEF)
			print_with_offset(aq_data->temp[n], aq_sett->temp_offset[n], temp_unit);
		else
			printf("   not connected");
		putchar('\n');
	}
}


void print_fans(aq5_data_t *aq_data, aq5_settings_t *aq_sett)
{
	printf("------------ Fans ------------\n");
	if (!out_all) {
		for (int n=0; n<AQ5_NUM_FAN; n++) {
			if (aq_data->fan_current[n]>0) {
				printf("Fan %2d:   %4drpm @ %3u%% %2.0f %s\n", n+1,
						aq_data->fan_rpm[n], aq_data->fan_duty[n],
						aq_data->fan_vrm_temp[n], temp_unit);
			}
		}
	} else {
		for (int n=0; n<AQ5_NUM_FAN; n++) {
			if (aq_data->fan_vrm_temp[n] != AQ5_FLOAT_UNDEF) {
				printf("Fan %2d:   %4drpm @ %3u%% %5.2f V\n", n+1,
					aq_data->fan_rpm[n], aq_data->fan_duty[n],
					aq_data->fan_voltage[n]);
				printf("               %4d mA  %5.2f %s\n", aq_data->fan_current[n],
						aq_data->fan_vrm_temp[n], temp_unit);
			} else {
				printf("Fan %2d:            not connected\n", n+1);
			}
		}
	}
}


void print_flow(aq5_data_t *aq_data, aq5_settings_t *aq_sett)
{
	printf("-------- Flow Sensors --------\n");
	for (int n=0; n<AQ5_NUM_FLOW; n++)
		if (aq_data->flow[n] != 0 || out_all)
			printf("Flow %2d:%18.0f %s\n", n+1, aq_data->flow[n], flow_unit);
}


void print_levels(aq5_data_t *aq_data, aq5_settings_t *aq_sett)
{
	printf("------- Liquid Levels --------\n");
	for (int n=0; n<AQ5_NUM_LEVEL; n++)
			if (aq_data->level[n] != 0 || out_all)
				printf("Level %2d:%20.0f%%\n", n+1, aq_data->level[n]);
}


void print_settings(aq5_data_t *aq_data, aq5_settings_t *aq_sett)
{
	printf("---------- Settings ----------\n");
	/* TODO: implement me */
	printf("TODO\n");
}


void print_export(aq5_data_t *aq_data, aq5_settings_t *aq_sett)
{
	/* A klunky way to get the offset for the local time from UTC since mktime doesn't honor gmtoff */
	struct tm *local_aq_time, *systime;
	time_t aq_time_t, systime_t;
	aq_time_t = mktime(&aq_data->time_utc);
	time(&systime_t);
	systime = localtime(&systime_t);
	aq_time_t += systime->tm_gmtoff;
	local_aq_time = localtime(&aq_time_t);

	/* pre-convert times to strings */
	char time_local_str[21], uptime_str[51];
	strftime(time_local_str, 20, "%Y-%m-%d %H:%M:%S", local_aq_time);
	strftime(uptime_str, 50, "%dd %H:%M:%S", &aq_data->uptime);

	printf("SYS_TIME_LOCAL='%s'\n", time_local_str);
	printf("SYS_UPTIME='%s'\n", uptime_str);
	for (int n=0; n<AQ5_NUM_CPU; n++) {
		if (aq_data->cpu_temp[n] != AQ5_FLOAT_UNDEF)
			printf("SYS_TEMP_CPU%d=%.2f\n", n+1, aq_data->cpu_temp[n]);
			printf("SYS_TEMP_CPU%d_OFFS=%.2f\n", n+1,
					aq_sett->cpu_temp_offset[n]);
	}
	for (int n=0; n<AQ5_NUM_TEMP; n++)
		if (aq_data->temp[n] != AQ5_FLOAT_UNDEF)
			printf("TEMP%d=%.2f\n", n+1, aq_data->temp[n]);
	for (int n=0; n<AQ5_NUM_FAN; n++)
		if (aq_data->fan_vrm_temp[n] != AQ5_FLOAT_UNDEF) {
			printf("FAN%d_RPM=%d\n", n+1, aq_data->fan_rpm[n]);
			printf("FAN%d_DUTY_CYCLE=%d\n", n+1, aq_data->fan_duty[n]);

		}
	for (int n=0; n<AQ5_NUM_FLOW; n++) {
		if (aq_data->flow[n] != AQ5_FLOAT_UNDEF)
			printf("FLOW%d=%.1f\n", n+1, aq_data->flow[n]);
	}


	if (out_all) {
		char time_utc_str[21], uptime_total_str[51];
		strftime(time_utc_str, 20, "%Y-%m-%d %H:%M:%S", &aq_data->time_utc);
		strftime(uptime_total_str, 50, "%yy %dd %H:%M:%S", &aq_data->total_time);
		printf("SYS_TIME_UTC='%s'\n", time_utc_str);
		printf("SYS_UPTIME_TOTAL='%s'\n", uptime_total_str);
		printf("SYS_SERIAL=%d-%d\n", aq_data->serial_major,
				aq_data->serial_minor);
		printf("SYS_FW=%d\n", aq_data->firmware_version);
		printf("SYS_LOADER=%d\n", aq_data->bootloader_version);
		printf("SYS_HW=%d\n", aq_data->hardware_version);
		printf("SYS_UNIT_TEMP='%s'\n", temp_unit);
		printf("SYS_UNIT_FLOW='%s'\n", flow_unit);
		printf("SYS_UNIT_PRESS='%s'\n", libaquaero5_get_string(aq_sett->pressure_units, PRESSURE_UNITS));
		printf("SYS_LANG='%s'\n", libaquaero5_get_string(aq_sett->language, LANGUAGE));
		printf("SYS_DEC_SEP='%s'\n", libaquaero5_get_string(aq_sett->decimal_separator, DECIMAL_SEPARATOR));
		printf("SYS_TIME_FMT='%s'\n", libaquaero5_get_string(aq_sett->time_format, TIME_FORMAT));
		printf("SYS_DATE_FMT='%s'\n", libaquaero5_get_string(aq_sett->date_format, DATE_FORMAT));
		printf("SYS_TIME_ADST='%s'\n", libaquaero5_get_string(aq_sett->auto_dst, AUTO_DST));
		printf("SYS_TIME_ZONE=%d\n", aq_sett->time_zone);
		printf("SYS_STBY_ACT_OFF='%s'\n", libaquaero5_get_string(aq_sett->standby_action_at_power_down, EVENT_ACTION));
		printf("SYS_STBY_ACT_ON='%s'\n", libaquaero5_get_string(aq_sett->standby_action_at_power_up, EVENT_ACTION));
		printf("KEYS_DISABLE='%s'\n", libaquaero5_get_string(aq_sett->disable_keys, DISABLE_KEYS));
		printf("KEYS_BEEP='%s'\n", libaquaero5_get_string(aq_sett->key_tone, KEY_TONE));
		printf("KEYS_BRIGHT_STBY=%d\n", aq_sett->standby_key_backlight_brightness);
		printf("KEY_SENS_DOWN=%d\n", aq_sett->key_sensitivity.down_key);
		printf("KEY_SENS_ENTR=%d\n", aq_sett->key_sensitivity.enter_key);
		printf("KEY_SENS_UP=%d\n", aq_sett->key_sensitivity.up_key);
		printf("KEY_SENS_P1=%d\n", aq_sett->key_sensitivity.programmable_key_1);
		printf("KEY_SENS_P2=%d\n", aq_sett->key_sensitivity.programmable_key_2);
		printf("KEY_SENS_P3=%d\n", aq_sett->key_sensitivity.programmable_key_3);
		printf("KEY_SENS_P4=%d\n", aq_sett->key_sensitivity.programmable_key_4);
		printf("DISP_ILLUM_MODE='%s'\n", libaquaero5_get_string(aq_sett->illumination_mode, ILLUM_MODE));
		printf("DISP_CONTRAST=%d\n", aq_sett->display_contrast);
		printf("DISP_CONTRAST_STBY=%d\n", aq_sett->standby_display_contrast);
		printf("DISP_BRIGHT_USE=%d\n", aq_sett->display_brightness_while_in_use);
		printf("DISP_BRIGHT_IDLE=%d\n", aq_sett->display_brightness_while_idle);
		printf("DISP_BRIGHT_TIME=%d\n", aq_sett->display_illumination_time);
		printf("DISP_BRIGHT_STBY=%d\n", aq_sett->standby_lcd_backlight_brightness);
		printf("DISP_BRIGHT_STBY_TIME=%d\n", aq_sett->standby_timeout_key_and_display_brightness);
		printf("DISP_MODE='%s'\n", libaquaero5_get_string(aq_sett->display_mode, DISPLAY_MODE));
		printf("DISP_TIME_MENU=%d\n", aq_sett->menu_display_duration);
		printf("DISP_TIME_APAGE=%d\n", aq_sett->display_duration_after_page_selection);
		for (int n=0; n<AQ5_NUM_INFO_PAGE; n++) {
			printf("PAGE%d_MODE='%s'\n", n+1, libaquaero5_get_string(aq_sett->info_page[n].page_display_mode, PAGE_DISPLAY_MODE));
			printf("PAGE%d_TIME=%d\n", n+1, aq_sett->info_page[n].display_time);
			printf("PAGE%d_TYPE='%s'\n", n+1, libaquaero5_get_string(aq_sett->info_page[n].info_page_type, INFO_SCREEN));
		}
		for (int n=0; n<AQ5_NUM_TEMP; n++)
			if (aq_data->temp[n] != AQ5_FLOAT_UNDEF)
				printf("TEMP%d_OFFS=%.2f\n", n+1, aq_sett->temp_offset[n]);
		for (int n=0; n<AQ5_NUM_VIRT_SENSORS; n++) {
			printf("VIRT_SENSOR%d_DATA_SOURCE_1='%s'\n", n+1, libaquaero5_get_string(aq_sett->virt_sensor_config[n].data_source_1, SENSOR_DATA_SOURCE));
			printf("VIRT_SENSOR%d_DATA_SOURCE_2='%s'\n", n+1, libaquaero5_get_string(aq_sett->virt_sensor_config[n].data_source_2, SENSOR_DATA_SOURCE));
			printf("VIRT_SENSOR%d_DATA_SOURCE_3='%s'\n", n+1, libaquaero5_get_string(aq_sett->virt_sensor_config[n].data_source_3, SENSOR_DATA_SOURCE));
			printf("VIRT_SENSOR%d_MODE='%s'\n", n+1, libaquaero5_get_string(aq_sett->virt_sensor_config[n].mode, VIRT_SENSOR_MODE));
		}
		for (int n=0; n<AQ5_NUM_SOFT_SENSORS; n++) {
			printf("SOFT_SENSOR%d_STATE='%s'\n", n+1, libaquaero5_get_string(aq_sett->soft_sensor_state[n], SOFT_SENSOR_STATE));
			printf("SOFT_SENSOR%d_FALLBACK_VALUE=%.2f\n", n+1, aq_sett->soft_sensor_fallback_value[n]);
			printf("SOFT_SENSOR%d_TIMEOUT=%d\n", n+1, aq_sett->soft_sensor_timeout[n]);
		}
		for (int n=0; n<AQ5_NUM_FLOW; n++) {
			printf("FLOW_SENSOR%d_CALIBRATION_VALUE=%d\n", n+1, aq_sett->flow_sensor_calibration_value[n]);
			printf("FLOW_SENSOR%d_LOWER_LIMIT=%.1f\n", n+1, aq_sett->flow_sensor_lower_limit[n]);
			printf("FLOW_SENSOR%d_UPPER_LIMIT=%.1f\n", n+1, aq_sett->flow_sensor_upper_limit[n]);
		}
		for (int n=0; n<AQ5_NUM_POWER_SENSORS; n++) {
			printf("POWER_SENSOR%d_FLOW_SENSOR=%s\n", n+1, libaquaero5_get_string(aq_sett->power_consumption_config[n].flow_sensor_data_source, FLOW_DATA_SOURCE));
			printf("POWER_SENSOR%d_TEMP_SENSOR_1=%s\n", n+1, libaquaero5_get_string(aq_sett->power_consumption_config[n].temp_sensor_1, SENSOR_DATA_SOURCE));
			printf("POWER_SENSOR%d_TEMP_SENSOR_2=%s\n", n+1, libaquaero5_get_string(aq_sett->power_consumption_config[n].temp_sensor_2, SENSOR_DATA_SOURCE));
		}
		for (int n=0; n<AQ5_NUM_FAN; n++) {
			printf("FAN%d_CURRENT=%d\n", n+1, aq_data->fan_current[n]);
			printf("FAN%d_VOLTAGE=%.2f\n", n+1, aq_data->fan_voltage[n]);
			printf("FAN%d_VRM_TEMP=%.2f\n", n+1, aq_data->fan_vrm_temp[n]);
			printf("FAN%d_VRM_TEMP_OFFS=%.2f\n", n+1, aq_sett->fan_vrm_temp_offset[n]);
			printf("FAN%d_MIN_RPM=%d\n", n+1, aq_sett->fan_min_rpm[n]);
			printf("FAN%d_MIN_DUTY=%d\n", n+1, aq_sett->fan_min_duty[n]);
			printf("FAN%d_MAX_RPM=%d\n", n+1, aq_sett->fan_max_rpm[n]);
			printf("FAN%d_MAX_DUTY=%d\n", n+1, aq_sett->fan_max_duty[n]);
			printf("FAN%d_BOOST_DUTY=%d\n", n+1, aq_sett->fan_startboost_duty[n]);
			printf("FAN%d_BOOST_DURATION=%d\n", n+1, aq_sett->fan_startboost_duration[n]);
			printf("FAN%d_PULSES=%d\n", n+1, aq_sett->fan_pulses_per_revolution[n]);
			printf("FAN%d_MODE=%d\n", n+1, aq_sett->fan_control_mode[n].fan_regulation_mode);
			printf("FAN%d_BOOST_ENABLED=%d\n", n+1, aq_sett->fan_control_mode[n].use_startboost);
			printf("FAN%d_FUSE_ENABLED=%d\n", n+1, aq_sett->fan_control_mode[n].use_programmable_fuse);
			printf("FAN%d HOLD_MIN_DUTY=%d\n", n+1, aq_sett->fan_control_mode[n].hold_minimum_power);
			printf("FAN%d_DATA_SRC='%s'\n", n+1, libaquaero5_get_string(aq_sett->fan_data_source[n], CONTROLLER_DATA_SRC));
			printf("FAN%d_FUSE_CURRENT=%d\n", n+1, aq_sett->fan_programmable_fuse[n]);
		}
		printf("POWER_OUTPUT1_MIN_POWER=%d\n", aq_sett->power_output_1_config.min_power);
		printf("POWER_OUTPUT1_MAX_POWER=%d\n", aq_sett->power_output_1_config.max_power);
		printf("POWER_OUTPUT1_DATA_SOURCE='%s'\n", libaquaero5_get_string(aq_sett->power_output_1_config.data_source, CONTROLLER_DATA_SRC));
		printf("POWER_OUTPUT1_HOLD_MINIMUM_POWER='%s'\n", libaquaero5_get_string(aq_sett->power_output_1_config.mode, POWER_OUTPUT_MODE));
		printf("POWER_OUTPUT2_MIN_POWER=%d\n", aq_sett->power_output_2_config.min_power);
		printf("POWER_OUTPUT2_MAX_POWER=%d\n", aq_sett->power_output_2_config.max_power);
		printf("POWER_OUTPUT2_DATA_SOURCE='%s'\n", libaquaero5_get_string(aq_sett->power_output_2_config.data_source, CONTROLLER_DATA_SRC));
		printf("POWER_OUTPUT2_HOLD_MINIMUM_POWER='%s'\n", libaquaero5_get_string(aq_sett->power_output_2_config.mode, POWER_OUTPUT_MODE));
		printf("AQUAERO_RELAY_DATA_SOURCE='%s'\n", libaquaero5_get_string(aq_sett->aquaero_relay_data_source, CONTROLLER_DATA_SRC));
		printf("AQUAERO_RELAY_CONFIG='%s'\n", libaquaero5_get_string(aq_sett->aquaero_relay_configuration, AQ_RELAY_CONFIG));
		printf("AQUAERO_RELAY_SWITCH_THRESHOLD=%d\n", aq_sett->aquaero_relay_switch_threshold);

		printf("RGB_LED_DATA_SOURCE='%s'\n", libaquaero5_get_string(aq_sett->rgb_led_controller_config.data_source, SENSOR_DATA_SOURCE));
		printf("RGB_LED_PULSATING_BRIGHTNESS='%s'\n", libaquaero5_get_string(aq_sett->rgb_led_controller_config.pulsating_brightness, LED_PB_MODE));
		printf("RGB_LED_LOW_TEMP_TEMP=%.2f\n", aq_sett->rgb_led_controller_config.low_temp.temp);
		printf("RGB_LED_LOW_TEMP_RED_LED=%d\n", aq_sett->rgb_led_controller_config.low_temp.red_led);
		printf("RGB_LED_LOW_TEMP_GREEN_LED=%d\n", aq_sett->rgb_led_controller_config.low_temp.green_led);
		printf("RGB_LED_LOW_TEMP_BLUE_LED=%d\n", aq_sett->rgb_led_controller_config.low_temp.blue_led);
		printf("RGB_LED_OPTIMUM_TEMP_TEMP=%.2f\n", aq_sett->rgb_led_controller_config.optimum_temp.temp);
		printf("RGB_LED_OPTIMUM_TEMP_RED_LED=%d\n", aq_sett->rgb_led_controller_config.optimum_temp.red_led);
		printf("RGB_LED_OPTIMUM_TEMP_GREEN_LED=%d\n", aq_sett->rgb_led_controller_config.optimum_temp.green_led);
		printf("RGB_LED_OPTIMUM_TEMP_BLUE_LED=%d\n", aq_sett->rgb_led_controller_config.optimum_temp.blue_led);
		printf("RGB_LED_HIGH_TEMP_TEMP=%.2f\n", aq_sett->rgb_led_controller_config.high_temp.temp);
		printf("RGB_LED_HIGH_TEMP_RED_LED=%d\n", aq_sett->rgb_led_controller_config.high_temp.red_led);
		printf("RGB_LED_HIGH_TEMP_GREEN_LED=%d\n", aq_sett->rgb_led_controller_config.high_temp.green_led);
		printf("RGB_LED_HIGH_TEMP_BLUE_LED=%d\n", aq_sett->rgb_led_controller_config.high_temp.blue_led);
		for (int n=0; n<AQ5_NUM_LEVEL; n++)
			printf("LEVEL%d=%.2f\n", n+1, aq_data->level[n]);
		for (int n=0; n<AQ5_NUM_TWO_POINT_CONTROLLERS; n++) {
			printf("TWO_POINT_CONTROLLER%d_DATA_SOURCE='%s'\n", n+1, libaquaero5_get_string(aq_sett->two_point_controller_config[n].data_source, SENSOR_DATA_SOURCE));
			printf("TWO_POINT_CONTROLLER%d_UPPER_LIMIT=%.2f\n", n+1, aq_sett->two_point_controller_config[n].upper_limit);
			printf("TWO_POINT_CONTROLLER%d_LOWER_LIMIT=%.2f\n", n+1, aq_sett->two_point_controller_config[n].lower_limit);
		}
		for (int n=0; n<AQ5_NUM_PRESET_VAL_CONTROLLERS; n++)
			printf("PRESET_VAL_CONTROLLER%d=%d\n", n+1, aq_sett->preset_value_controller[n]);
		for (int n=0; n<AQ5_NUM_TARGET_VAL_CONTROLLERS; n++) {
			printf("TARGET_VAL_CONTROLLER%d_DATA_SOURCE='%s'\n", n+1, libaquaero5_get_string(aq_sett->target_value_controller_config[n].data_source, SENSOR_DATA_SOURCE));
			printf("TARGET_VAL_CONTROLLER%d_TARGET_VALUE=%.2f\n", n+1, aq_sett->target_value_controller_config[n].target_val);
			printf("TARGET_VAL_CONTROLLER%d_FACTOR_P=%d\n", n+1, aq_sett->target_value_controller_config[n].factor_p);
			printf("TARGET_VAL_CONTROLLER%d_FACTOR_I=%.1f\n", n+1, aq_sett->target_value_controller_config[n].factor_i);
			printf("TARGET_VAL_CONTROLLER%d_FACTOR_D=%d\n", n+1, aq_sett->target_value_controller_config[n].factor_d);
			printf("TARGET_VAL_CONTROLLER%d_RESET_TIME=%.1f\n", n+1, aq_sett->target_value_controller_config[n].reset_time);
			printf("TARGET_VAL_CONTROLLER%d_HYSTERESIS=%.2f\n", n+1, aq_sett->target_value_controller_config[n].hysteresis);
		}
		for (int n=0; n<AQ5_NUM_CURVE_CONTROLLERS; n++) {
			printf("CURVE_CONTROLLER%d_DATA_SOURCE='%s'\n", n+1, libaquaero5_get_string(aq_sett->curve_controller_config[n].data_source, SENSOR_DATA_SOURCE));
			printf("CURVE_CONTROLLER%d_STARTUP_TEMP=%.2f\n", n+1, aq_sett->curve_controller_config[n].startup_temp);
			for (int m=0; m<AQ5_NUM_CURVE_POINTS; m++) {
				printf("CURVE_CONTROLLER%d_POINT%d_TEMP=%.2f\n", n+1, m+1, aq_sett->curve_controller_config[n].curve_point[m].temp);
				printf("CURVE_CONTROLLER%d_POINT%d_PERCENT=%d\n", n+1, m+1, aq_sett->curve_controller_config[n].curve_point[m].percent);
			}
		}
		for (int n=0; n<AQ5_NUM_DATA_LOG; n++) {
			printf("DATA_LOG%d_INTERVAL='%s'\n", n+1, libaquaero5_get_string(aq_sett->data_log_config[n].interval, DATA_LOG_INTERVAL));
			printf("DATA_LOG%d_DATA_SOURCE='%s'\n", n+1, libaquaero5_get_string(aq_sett->data_log_config[n].data_source, SENSOR_DATA_SOURCE));
		}
		for (int n=0; n<AQ5_NUM_ALARM_AND_WARNING_LVLS; n++) {
			for (int m=0; m<3; m++) {
				switch (n) {
					case 0:
						printf("ALARM_NORMAL_ACTION%d='%s'\n", m+1, libaquaero5_get_string(aq_sett->alarm_and_warning_level[n].action[m], EVENT_ACTION));
						break;
					case 1:
						printf("ALARM_WARNING_ACTION%d='%s'\n", m+1, libaquaero5_get_string(aq_sett->alarm_and_warning_level[n].action[m], EVENT_ACTION));
						break;
					case 2:
						printf("ALARM_ALARM_ACTION%d='%s'\n", m+1, libaquaero5_get_string(aq_sett->alarm_and_warning_level[n].action[m], EVENT_ACTION));
						break;
					default:
						printf("ALARM_WARNING%d_ACTION%d='%s'\n", n+1, m+1, libaquaero5_get_string(aq_sett->alarm_and_warning_level[n].action[m], EVENT_ACTION));
				}
			}
		}
		printf("ALARM_SUPPRESS_AT_POWERON=%d\n", aq_sett->suppress_alarm_at_poweron);
		for (int n=0; n<AQ5_NUM_TEMP_ALARMS; n++) {
			printf("TEMP_ALARM%d_DATA_SOURCE='%s'\n", n+1, libaquaero5_get_string(aq_sett->temp_alarm[n].data_source, SENSOR_DATA_SOURCE));
			printf("TEMP_ALARM%d_CONFIG='%s'\n", n+1, libaquaero5_get_string(aq_sett->temp_alarm[n].config, TEMP_ALARM_CONFIG));
			printf("TEMP_ALARM%d_LIMIT_FOR_WARNING=%.2f\n", n+1, aq_sett->temp_alarm[n].limit_for_warning);
			printf("TEMP_ALARM%d_SET_WARNING_LEVEL='%s'\n", n+1, libaquaero5_get_string(aq_sett->temp_alarm[n].set_warning_level, ALARM_WARNING_LEVELS));
			printf("TEMP_ALARM%d_LIMIT_FOR_ALARM=%.2f\n", n+1, aq_sett->temp_alarm[n].limit_for_alarm);
			printf("TEMP_ALARM%d_SET_ALARM_LEVEL='%s'\n", n+1, libaquaero5_get_string(aq_sett->temp_alarm[n].set_alarm_level, ALARM_WARNING_LEVELS));
		}
		for (int n=0; n<AQ5_NUM_FAN; n++) {
			printf("FAN%d_ALARM_LIMIT_FOR_WARNING='%s'\n", n+1, libaquaero5_get_string(aq_sett->fan_alarm[n].limit_for_warning, FAN_LIMITS));
			printf("FAN%d_SET_WARNING_LEVEL='%s'\n", n+1, libaquaero5_get_string(aq_sett->fan_alarm[n].set_warning_level, ALARM_WARNING_LEVELS));
			printf("FAN%d_ALARM_LIMIT_FOR_ALARM='%s'\n", n+1, libaquaero5_get_string(aq_sett->fan_alarm[n].limit_for_alarm, FAN_LIMITS));
			printf("FAN%d_SET_ALARM_LEVEL='%s'\n", n+1, libaquaero5_get_string(aq_sett->fan_alarm[n].set_alarm_level, ALARM_WARNING_LEVELS));
		}
	}
}


int main(int argc, char *argv[])
{
	parse_cmdline(argc, argv);

	aq5_data_t aquaero_data;
	aq5_settings_t aquaero_settings;

	if (libaquaero5_poll(device, &aquaero_data, &err_msg) < 0) {
		fprintf(stderr, "failed to poll: %s (%s)\n", err_msg, strerror(errno));
		exit(EXIT_FAILURE);
	}
	if (err_msg != NULL)
		fprintf(stderr, "WARNING: %s\n", err_msg);

	if (libaquaero5_getsettings(NULL, &aquaero_settings, &err_msg) < 0) {
		fprintf(stderr, "failed to get settings: %s (%s)\n", err_msg,
				strerror(errno));
		exit(EXIT_FAILURE);
	}
	

	if (dump_data_file != NULL) {
		if (libaquaero5_dump_data(dump_data_file) != 0)
			fprintf(stderr, "failed to dump data to '%s': %s", dump_data_file,
					strerror(errno));
	}
	if (dump_settings_file != NULL) {
		if (libaquaero5_dump_settings(dump_settings_file) != 0)
			fprintf(stderr, "failed to dump settings to '%s': %s", dump_settings_file, 
					strerror(errno));
	}

	/* Set Unit Symbols */
	switch (aquaero_settings.temp_units) {
		case CELSIUS:		temp_unit = "°C"; break;
		case FAHRENHEIT:	temp_unit = "°F"; break;
		case KELVIN:		temp_unit = "°K"; break;
		default:			temp_unit = "°?"; break;
	}
	switch (aquaero_settings.flow_units) {
		case LPH:		flow_unit = "l/h"; break;
		case LPM:		flow_unit = "l/min"; break;
		case GPH_US:	flow_unit = "gph"; break;
		case GPM_US:	flow_unit = "gpm"; break;
		case GPH_IMP:	flow_unit = "gph"; break;
		case GPM_IMP:	flow_unit = "gpm"; break;
		default:		flow_unit = "??"; break;
	}

	if (out_format == F_STD) {
		/* Human-Readable Output, please */
		print_system(&aquaero_data, &aquaero_settings);
		print_sensors(&aquaero_data, &aquaero_settings);
		print_fans(&aquaero_data, &aquaero_settings);
		print_flow(&aquaero_data, &aquaero_settings);
		print_levels(&aquaero_data, &aquaero_settings);
		if (out_all)
			print_settings(&aquaero_data, &aquaero_settings);
	} else if (out_format == F_SCRIPT) {
		/* Bunch of Bytes, please */
		print_export(&aquaero_data, &aquaero_settings);
	}

	/* Shut down communications and clean up */
	libaquaero5_exit();

	exit(EXIT_SUCCESS);
}

