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

typedef struct {
	uint32_t        days;
	uint32_t        hours;
	uint32_t        minutes;
	uint32_t        seconds;
} uptime_t;

typedef enum { M_STD, M_SCRIPT } out_mode_t;

char *device = NULL;
char *err_msg = NULL;
char *dump_data_file = NULL;
char *dump_settings_file = NULL;
out_mode_t out_mode = M_STD;


/* get the uptime for the given value in seconds */
inline void get_uptime(uint32_t timeval, uptime_t *uptime)
{
	uptime->seconds = timeval;
	uptime->minutes = uptime->seconds / 60;
	uptime->hours = uptime->minutes / 60;
	uptime->days = uptime->hours / 24;
	if (uptime->seconds > 59) 
		uptime->seconds -= 60 * uptime->minutes;
	if (uptime->minutes > 59)
		uptime->minutes -= 60 * uptime->hours;
	if (uptime->hours > 23)
		uptime->hours -= 24 * uptime->days;
}

void print_help()
{
	printf("Usage: aerocli [OPTIONS]\n\n");

	printf("Options:\n");
	printf("  -d  DEVICE use given DEVICE (no auto-discovery)\n");
	printf("  -o  MODE   output mode (default, export)\n");
	printf("  -D  FILE   dump data to FILE\n");
	printf("  -S  FILE   dump settings to FILE\n");
	printf("  -h         display this usage information\n");

	printf("\n");
	printf("This version of aerocli was built on %s %s.\n", __DATE__, __TIME__);
}


void parse_cmdline(int argc, char *argv[])
{
	char c;
	extern int optind, optopt, opterr;

	while ((c = getopt(argc, argv, "d:o:D:S:h")) != -1) {
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
					out_mode = M_SCRIPT;
				} else if (strcmp(optarg, "default") == 0) {
					out_mode = M_STD;
				} else {
					fprintf(stderr, "invalid output format: '%s'\n", optarg);
					exit(EXIT_FAILURE);
				}
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


int main(int argc, char *argv[])
{
	parse_cmdline(argc, argv);

	aq5_data_t aquaero_data;
	aq5_settings_t aquaero_settings;

	if (libaquaero5_poll(device, &aquaero_data, &err_msg) < 0) {
		fprintf(stderr, "failed to poll: %s (%s)\n", err_msg, strerror(errno));
		exit(EXIT_FAILURE);
	}

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

	/* output mode changes format strings */
	const char *temp_fstr, *fan_vrm_temp_fstr, *fan_current_fstr, *fan_rpm_fstr, *fan_duty_cycle_fstr, *fan_voltage_fstr, *flow_fstr, *cpu_temp_fstr, *level_fstr, *settings_fan_min_rpm_fstr, *settings_fan_max_rpm_fstr, *settings_fan_min_duty_cycle_fstr, *settings_fan_max_duty_cycle_fstr, *settings_fan_startboost_duty_cycle_fstr, *settings_fan_startboost_duration_fstr, *settings_fan_pulses_per_revolution_fstr, *settings_fan_programmable_fuse_fstr, *settings_fan_data_source_fstr, *settings_fan_control_regulation_mode_fstr, *settings_fan_control_use_startboost_fstr, *settings_fan_control_use_fuse_fstr, *settings_fan_control_hold_min_power_fstr;;

	struct tm aq_time, *local_aq_time, *systime;
	time_t aq_time_t, systime_t;
	/* offset time from 00:00:00 1/1/2009 */
	aq_time.tm_min = 0;
	aq_time.tm_hour = 0;
	aq_time.tm_mday = 1;
	aq_time.tm_mon = 0;
	aq_time.tm_year = 109; 
	aq_time.tm_gmtoff = 0; 
	aq_time.tm_sec = aquaero_data.current_time;
	aq_time_t = mktime(&aq_time);

	/* A klunky way to get the offset for the local time from UTC since mktime doesn't honor gmtoff */
	time(&systime_t);
	systime = localtime(&systime_t);
	aq_time_t += systime->tm_gmtoff; 
	local_aq_time = localtime(&aq_time_t);

	/* Get the uptimes */
	uptime_t uptime, total_time;

	get_uptime(aquaero_data.uptime, &uptime);
	get_uptime(aquaero_data.total_time, &total_time);

	switch (out_mode) {
		case M_STD:
			printf("Time (UTC) = %s", asctime(&aq_time));
			printf("Time (local) = %s", asctime(local_aq_time));
			printf("Serial number = %d-%d\n", aquaero_data.serial_major, aquaero_data.serial_minor);
			printf("Firmware version = %d\n", aquaero_data.firmware_version);
			printf("Bootloader version = %d\n", aquaero_data.bootloader_version);
			printf("Hardware version = %d\n", aquaero_data.hardware_version);
			printf("Uptime = %d days, %d hours, %d minutes, %d seconds\n", uptime.days, uptime.hours, uptime.minutes, uptime.seconds);
			printf("Total time = %d days, %d hours, %d minutes, %d seconds\n", total_time.days, total_time.hours, total_time.minutes, total_time.seconds);
			temp_fstr = "temp%d: %.2f °C\n";
			fan_vrm_temp_fstr = "fan%d VRM temp: %.2f °C\n";
			fan_current_fstr = "fan%d current: %d mA\n";
			fan_rpm_fstr = "fan%d RPM: %d rpm\n";
			fan_duty_cycle_fstr = "fan%d duty cycle: %.2f %%\n";
			fan_voltage_fstr = "fan%d voltage: %.2f V\n";
			flow_fstr = "flow%d: %.1f l/h\n";
			cpu_temp_fstr = "CPU%d temp: %.2f °C\n";
			level_fstr = "level%d: %.2f %%\n";
			settings_fan_min_rpm_fstr = "fan%d minimum RPM: %d rpm\n";
			settings_fan_max_rpm_fstr = "fan%d maximum RPM: %d rpm\n";
			settings_fan_min_duty_cycle_fstr = "fan%d minimum duty cycle: %.2f %%\n";
			settings_fan_max_duty_cycle_fstr = "fan%d maximum duty cycle: %.2f %%\n";
			settings_fan_startboost_duty_cycle_fstr = "fan%d startboost duty cycle: %.2f %%\n";
			settings_fan_startboost_duration_fstr = "fan%d startboost duration: %d seconds\n";
			settings_fan_pulses_per_revolution_fstr = "fan%d pulses per revolution: %d\n";
			settings_fan_control_regulation_mode_fstr = "fan%d regulation mode: %d\n";
			settings_fan_control_use_startboost_fstr = "fan%d use startboost: %d\n";
			settings_fan_control_use_fuse_fstr = "fan%d use programmable fuse: %d\n";
			settings_fan_control_hold_min_power_fstr = "fan%d hold minimum power: %d\n";
			settings_fan_data_source_fstr = "fan%d data source: %s\n";
			/* settings_fan_data_source_fstr = "fan%d data source: 0x%x\n"; */
			settings_fan_programmable_fuse_fstr = "fan%d programmable fuse: %d mA\n";
			break;
		case M_SCRIPT:
		default:
			temp_fstr = "TEMP%d=%.2f\n";
			fan_vrm_temp_fstr = "FAN%d_VRM_TEMP=%.2f\n";
			fan_current_fstr = "FAN%d_CURRENT=%d\n";
			fan_rpm_fstr = "FAN%d_RPM=%d\n";
			fan_duty_cycle_fstr = "FAN%d_DUTY_CYCLE=%.2f\n";
			fan_voltage_fstr = "FAN%d_VOLTAGE=%.2f\n";
			flow_fstr = "FLOW%d=%.1f\n";
			cpu_temp_fstr = "CPU%d_TEMP=%.2f\n";
			level_fstr = "LEVEL%d=%.2f\n";
			break;
	}

	if (1) { /* print_temp */
		for (int n=0; n<AQ5_NUM_TEMP; n++)
			if (aquaero_data.temp[n] != AQ5_FLOAT_UNDEF)
				printf(temp_fstr, n+1, aquaero_data.temp[n]);
	}

	if (1) { /* print_fan */
		for (int n=0; n<AQ5_NUM_FAN; n++) {
			if (aquaero_data.fan_vrm_temp[n] != AQ5_FLOAT_UNDEF) {
				printf(fan_vrm_temp_fstr, n+1, aquaero_data.fan_vrm_temp[n]);
				printf(fan_current_fstr, n+1, aquaero_data.fan_current[n]);
				printf(fan_rpm_fstr, n+1, aquaero_data.fan_rpm[n]);
				printf(fan_duty_cycle_fstr, n+1, aquaero_data.fan_duty_cycle[n]);
				printf(fan_voltage_fstr, n+1, aquaero_data.fan_voltage[n]);
			}
		}
	}

	if (1) { /* print flow */
		for (int n=0; n<AQ5_NUM_FLOW; n++) {
			printf(flow_fstr, n+1, aquaero_data.flow[n]);
		}
	}

	if (1) { /* print CPU temp */
		for (int n=0; n<AQ5_NUM_CPU; n++) {
			if (aquaero_data.cpu_temp[n] != AQ5_FLOAT_UNDEF)
				printf(cpu_temp_fstr, n+1, aquaero_data.cpu_temp[n]);
		}
	}

	if (1) { /* print liquid level */
		for (int n=0; n<AQ5_NUM_LEVEL; n++) {
			printf(level_fstr, n+1, aquaero_data.level[n]);
		}
	}

	/* print settings */
	if (out_mode != M_SCRIPT) {
		printf("\n------Settings------\n");
	
		for (int n=0; n<AQ5_NUM_FAN; n++) {
			printf(settings_fan_min_rpm_fstr, n+1, aquaero_settings.fan_min_rpm[n]);
			printf(settings_fan_max_rpm_fstr, n+1, aquaero_settings.fan_max_rpm[n]);
			printf(settings_fan_min_duty_cycle_fstr, n+1, aquaero_settings.fan_min_duty_cycle[n]);
			printf(settings_fan_max_duty_cycle_fstr, n+1, aquaero_settings.fan_max_duty_cycle[n]);
			printf(settings_fan_startboost_duty_cycle_fstr, n+1, aquaero_settings.fan_startboost_duty_cycle[n]);
			printf(settings_fan_startboost_duration_fstr, n+1, aquaero_settings.fan_startboost_duration[n]);
			printf(settings_fan_pulses_per_revolution_fstr, n+1, aquaero_settings.fan_pulses_per_revolution[n]);
			printf(settings_fan_control_regulation_mode_fstr, n+1, aquaero_settings.fan_control_mode[n].fan_regulation_mode);
			printf(settings_fan_control_use_startboost_fstr, n+1, aquaero_settings.fan_control_mode[n].use_startboost);
			printf(settings_fan_control_use_fuse_fstr, n+1, aquaero_settings.fan_control_mode[n].use_programmable_fuse);
			printf(settings_fan_control_hold_min_power_fstr, n+1, aquaero_settings.fan_control_mode[n].hold_minimum_power);
			/* printf(settings_fan_data_source_fstr, n+1, aquaero_settings.fan_data_source[n]); */
			printf(settings_fan_data_source_fstr, n+1, aq5_get_fan_data_source_string(aquaero_settings.fan_data_source[n]));
			printf(settings_fan_programmable_fuse_fstr, n+1, aquaero_settings.fan_programmable_fuse[n]);
		}
	}

	/* Shut down communications and clean up */
	libaquaero5_exit();

	exit(EXIT_SUCCESS);
}

