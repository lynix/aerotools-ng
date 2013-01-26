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

#include "libaquaero5.h"


unsigned char buffer[AQ5_DATA_LEN];

/* local functions */ 
static int aquaero_get_report(int fd, int report_id, unsigned report_type);

inline int aq5_get_int(unsigned char *buffer, short offset)
{
	return (buffer[offset] << 8) | buffer[offset + 1];
}

inline uint32_t aq5_get_int32(unsigned char *buffer, short offset)
{
	return (buffer[offset] << 24) | (buffer[offset + 1] << 16) | (buffer[offset + 2] << 8) | buffer[offset + 3];
}

int libaquaero5_poll(char *device, aq5_data_t *data_dest)
{
	int fd = open(device, O_RDONLY);
	int res;
	struct hiddev_devinfo dinfo;
	struct hiddev_string_descriptor hStr;

	if (fd < 0)
		return -1;

	ioctl(fd, HIDIOCGDEVINFO, &dinfo);
	/* printf("HID: vendor 0x%x product 0x%x(0x%x) version 0x%x\n", dinfo.vendor, dinfo.product & 0xffff, dinfo.product, dinfo.version); */
	if ((dinfo.vendor != 0xc70) || ((dinfo.product & 0xffff) != 0xf001)) {
		printf("No Aquaero 5 found on %s\n", device);
		close(fd);
		return -1;
	}

	hStr.index = 2; /* Vendor = 1, Product = 2 */
	hStr.value[0] = 0;
	ioctl(fd, HIDIOCGSTRING, &hStr);
	printf("Found '%s'\n", hStr.value);
	res = aquaero_get_report(fd, 0x1, HID_REPORT_TYPE_INPUT);
	if (res == 0) {
		close(fd);
		printf("failed to get report!\n");
		return -1;	
	}
	close(fd);

	/* current time */
	data_dest->current_time = aq5_get_int32(buffer, AQ5_CURRENT_TIME_OFFS);

	/* device info */
	data_dest->serial_major = aq5_get_int(buffer, AQ5_SERIAL_MAJ_OFFS);
	data_dest->serial_minor = aq5_get_int(buffer, AQ5_SERIAL_MIN_OFFS);
	data_dest->firmware_version = aq5_get_int(buffer, AQ5_FIRMWARE_VER_OFFS);
	data_dest->bootloader_version = aq5_get_int(buffer, AQ5_BOOTLOADER_VER_OFFS);
	data_dest->hardware_version = aq5_get_int(buffer, AQ5_HARDWARE_VER_OFFS);

	/* operating times */
	data_dest->uptime = aq5_get_int32(buffer, AQ5_UPTIME_OFFS);
	data_dest->total_time = aq5_get_int32(buffer, AQ5_TOTAL_TIME_OFFS);

	/* temperature sensors */
	int n;
	for (int i=0; i<AQ5_NUM_TEMP; i++) {
		n = aq5_get_int(buffer, AQ5_TEMP_OFFS  + i * AQ5_TEMP_DIST);
		data_dest->temp[i] = n!=AQ5_TEMP_UNDEF ? (double)n/100.0 : AQ_TEMP_UNDEF;
	}

	/* fans */
	for (int i=0; i<AQ5_NUM_FAN; i++) {
		n = aq5_get_int(buffer, AQ5_FAN_VRM_OFFS + i * AQ5_FAN_VRM_DIST);
		data_dest->fan_vrm_temp[i] = n!=AQ5_FAN_VRM_UNDEF ? (double)n/100.0 : AQ_TEMP_UNDEF;
		data_dest->fan_rpm[i] = aq5_get_int(buffer, AQ5_FAN_OFFS + i * AQ5_FAN_DIST);
		data_dest->fan_duty_cycle[i] = (double)aq5_get_int(buffer, AQ5_FAN_OFFS + 2 + i * AQ5_FAN_DIST) / 100.0;
		data_dest->fan_voltage[i] = (double)aq5_get_int(buffer, AQ5_FAN_OFFS + 4 + i * AQ5_FAN_DIST) / 100.0;
		data_dest->fan_current[i] = (double)aq5_get_int(buffer, AQ5_FAN_OFFS + 6 + i * AQ5_FAN_DIST) / 10.0;
	}

	/* flow sensors */
	for (int i=0; i<AQ5_NUM_FLOW; i++) {
		data_dest->flow[i] = (double)aq5_get_int(buffer, AQ5_FLOW_OFFS + i * AQ5_FLOW_DIST) / 10.0;
	}

	/* CPU temp */
	for (int i=0; i<AQ5_NUM_CPU; i++) {
		n = (double)aq5_get_int(buffer, AQ5_CPU_TEMP_OFFS + i * AQ5_CPU_TEMP_DIST);
		data_dest->cpu_temp[i] = n!=AQ5_TEMP_UNDEF ? (double)n/100.0 : AQ_TEMP_UNDEF;
	}

	/* Liquid level sensors */
	for (int i=0; i<AQ5_NUM_LEVEL; i++) {
		data_dest->level[i] = (double)aq5_get_int(buffer, AQ5_LEVEL_OFFS + i * AQ5_LEVEL_DIST) / 100.0;
	}

	return 0;
}

/* Return the raw buffer data */
unsigned char *aquaero_get_buffer()
{
	return buffer;
}

/* Get the specified HID report */
static int aquaero_get_report(int fd, int report_id, unsigned report_type)
{
	struct hiddev_report_info rinfo;
	struct hiddev_field_info finfo;
	struct hiddev_usage_ref uref;
	int j, ret;

	rinfo.report_type = report_type;
	rinfo.report_id = report_id;
	ret = ioctl(fd, HIDIOCGREPORTINFO, &rinfo);
		/* printf("HIDIOCGREPORTINFO: report_id=0x%X (%u fields)\n", rinfo.report_id, rinfo.num_fields); */
			finfo.report_type = rinfo.report_type;
			finfo.report_id   = rinfo.report_id;
			if (rinfo.report_id == report_id) {
				finfo.field_index = 0; /* There is only one field for the Aquaero reports */
				ioctl(fd, HIDIOCGFIELDINFO, &finfo);
				/* Put the report ID into the first byte to be consistant with hidraw */
				buffer[0] = report_id;
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
					buffer[j+1] = uref.value;
				}
			}
	return j;
}
