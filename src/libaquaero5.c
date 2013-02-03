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

/*
 *	Global variables
 */

unsigned char aq5_buf_data[AQ5_DATA_LEN];
unsigned char aq5_buf_settings[AQ5_SETTINGS_LEN];
/* TODO: this really should be an array to handle more than one device */
int aq5_fd = -1;

/* 
 * 	Local functions 
 */ 

inline int aq5_get_int(unsigned char *buffer, short offset)
{
	return (buffer[offset] << 8) | buffer[offset + 1];
}

inline uint32_t aq5_get_int32(unsigned char *buffer, short offset)
{
	return (buffer[offset] << 24) | (buffer[offset + 1] << 16) |
			(buffer[offset + 2] << 8) | buffer[offset + 3];
}

/* Open the file handle t othe device */
int aq5_open(char *device)
{
	struct hiddev_devinfo dinfo;

	/* Only open the device if we need to */
	if (fcntl(aq5_fd, F_GETFL) != -1)
		/* The file handle is already defined and open, just continue */
		return 0;

	aq5_fd = open(device, O_RDONLY);
	if (fcntl(aq5_fd, F_GETFL) == -1) {
		/* We tried to open the device but failed */
		fprintf(stderr, "failed to open %s\n", device);
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
		libaquaero5_exit();
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
int libaquaero5_getsettings(char *device, aq5_settings_t *settings_dest)
{
	int res;

	/* Allow the device to be disconnected and open only if the fd is undefined */
	if (aq5_open(device) != 0) {
		return -1;
	}

	res = aq5_get_report(aq5_fd, 0xB, HID_REPORT_TYPE_FEATURE, aq5_buf_settings);
	if (res != 0) {
		libaquaero5_exit();
		printf("failed to get report!");
		return -1;	
	}

	/* fan settings */
	for (int i=0; i<AQ5_NUM_FAN; i++) {
		settings_dest->fan_min_rpm[i] = aq5_get_int(aq5_buf_settings, AQ5_SETTINGS_FAN_OFFS + i * AQ5_SETTINGS_FAN_DIST);
		settings_dest->fan_max_rpm[i] = aq5_get_int(aq5_buf_settings, AQ5_SETTINGS_FAN_OFFS + 2 + i * AQ5_SETTINGS_FAN_DIST);
		settings_dest->fan_min_duty_cycle[i] = (double)aq5_get_int(aq5_buf_settings, AQ5_SETTINGS_FAN_OFFS + 4 + i * AQ5_SETTINGS_FAN_DIST) /100.0;
		settings_dest->fan_max_duty_cycle[i] = (double)aq5_get_int(aq5_buf_settings, AQ5_SETTINGS_FAN_OFFS + 6 + i * AQ5_SETTINGS_FAN_DIST) /100.0;
		settings_dest->fan_startboost_duty_cycle[i] = (double)aq5_get_int(aq5_buf_settings, AQ5_SETTINGS_FAN_OFFS + 8 + i * AQ5_SETTINGS_FAN_DIST) /100.0;
		settings_dest->fan_startboost_duration[i] = aq5_get_int(aq5_buf_settings, AQ5_SETTINGS_FAN_OFFS + 10 + i * AQ5_SETTINGS_FAN_DIST);
		settings_dest->fan_pulses_per_revolution[i] = aq5_get_int(aq5_buf_settings, AQ5_SETTINGS_FAN_OFFS + 12 + i * AQ5_SETTINGS_FAN_DIST);
		/* two unknowns */
		settings_dest->fan_programmable_fuse[i] = aq5_get_int(aq5_buf_settings, AQ5_SETTINGS_FAN_OFFS + 18 + i * AQ5_SETTINGS_FAN_DIST);
	}

	return 0;
}

/* Get the current sensor data from input report 1 */
int libaquaero5_poll(char *device, aq5_data_t *data_dest)
{
	int res;

	/* Allow the device to be disconnected and open only if the fd is undefined */
	if (aq5_open(device) != 0) {
		return -1;
	}

	res = aq5_get_report(aq5_fd, 0x1, HID_REPORT_TYPE_INPUT, aq5_buf_data);
	if (res != 0) {
		libaquaero5_exit();
		printf("failed to get report!\n");
		return -1;	
	}

	/* current time */
	data_dest->current_time = aq5_get_int32(aq5_buf_data, AQ5_CURRENT_TIME_OFFS);

	/* device info */
	data_dest->serial_major = aq5_get_int(aq5_buf_data, AQ5_SERIAL_MAJ_OFFS);
	data_dest->serial_minor = aq5_get_int(aq5_buf_data, AQ5_SERIAL_MIN_OFFS);
	data_dest->firmware_version = aq5_get_int(aq5_buf_data, AQ5_FIRMWARE_VER_OFFS);
	data_dest->bootloader_version = aq5_get_int(aq5_buf_data, AQ5_BOOTLOADER_VER_OFFS);
	data_dest->hardware_version = aq5_get_int(aq5_buf_data, AQ5_HARDWARE_VER_OFFS);

	/* operating times */
	data_dest->uptime = aq5_get_int32(aq5_buf_data, AQ5_UPTIME_OFFS);
	data_dest->total_time = aq5_get_int32(aq5_buf_data, AQ5_TOTAL_TIME_OFFS);

	/* temperature sensors */
	int n;
	for (int i=0; i<AQ5_NUM_TEMP; i++) {
		n = aq5_get_int(aq5_buf_data, AQ5_TEMP_OFFS  + i * AQ5_TEMP_DIST);
		data_dest->temp[i] = n!=AQ5_TEMP_UNDEF ? (double)n/100.0 : AQ_TEMP_UNDEF;
	}

	/* fans */
	for (int i=0; i<AQ5_NUM_FAN; i++) {
		n = aq5_get_int(aq5_buf_data, AQ5_FAN_VRM_OFFS + i * AQ5_FAN_VRM_DIST);
		data_dest->fan_vrm_temp[i] = n!=AQ5_FAN_VRM_UNDEF ? (double)n/100.0 : AQ_TEMP_UNDEF;
		data_dest->fan_rpm[i] = aq5_get_int(aq5_buf_data, AQ5_FAN_OFFS + i * AQ5_FAN_DIST);
		data_dest->fan_duty_cycle[i] = (double)aq5_get_int(aq5_buf_data, AQ5_FAN_OFFS + 2 + i * AQ5_FAN_DIST) / 100.0;
		data_dest->fan_voltage[i] = (double)aq5_get_int(aq5_buf_data, AQ5_FAN_OFFS + 4 + i * AQ5_FAN_DIST) / 100.0;
		data_dest->fan_current[i] = aq5_get_int(aq5_buf_data, AQ5_FAN_OFFS + 6 + i * AQ5_FAN_DIST);
	}

	/* flow sensors */
	for (int i=0; i<AQ5_NUM_FLOW; i++) {
		data_dest->flow[i] = (double)aq5_get_int(aq5_buf_data, AQ5_FLOW_OFFS + i * AQ5_FLOW_DIST) / 10.0;
	}

	/* CPU temp */
	for (int i=0; i<AQ5_NUM_CPU; i++) {
		n = (double)aq5_get_int(aq5_buf_data, AQ5_CPU_TEMP_OFFS + i * AQ5_CPU_TEMP_DIST);
		data_dest->cpu_temp[i] = n!=AQ5_TEMP_UNDEF ? (double)n/100.0 : AQ_TEMP_UNDEF;
	}

	/* Liquid level sensors */
	for (int i=0; i<AQ5_NUM_LEVEL; i++) {
		data_dest->level[i] = (double)aq5_get_int(aq5_buf_data, AQ5_LEVEL_OFFS + i * AQ5_LEVEL_DIST) / 100.0;
	}

	return 0;
}

/* Return the raw buffer data */
unsigned char *libaquaero5_get_data_buffer()
{
	return aq5_buf_data;
}

/* Return the raw settings buffer data */
unsigned char *libaquaero5_get_settings_buffer()
{
	return aq5_buf_settings;
}

