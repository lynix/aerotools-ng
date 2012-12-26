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

#include "aerocli5.h"


typedef enum { M_STD, M_SCRIPT } out_mode_t;

int main(int argc, char *argv[])
{
	int	r = EXIT_SUCCESS;

	if (argc < 2) {
		fprintf(stderr, "%s: insufficient arguments.\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	out_mode_t out_mode = M_STD;

	if (argc == 3 && strcmp(argv[2], "--export") == 0)
		out_mode = M_SCRIPT;

	aq5_data_t aquaero_data;

	if (libaquaero5_poll(argv[1], &aquaero_data) < 0) {
		fprintf(stderr, "failed to poll '%s': %s\n", argv[1], strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (argc >= 3 && strcmp(argv[2], "--dump") == 0) {
		printf("Dumping data to %s\n", argv[3]);
		r = dump_data(argv[3], aquaero_get_buffer());
	} 

	/* output mode changes format strings */
	const char *temp_fstr, *fan_vrm_temp_fstr, *fan_current_fstr, *fan_rpm_fstr, *fan_duty_cycle_fstr, *fan_voltage_fstr, *flow_fstr;;

	switch (out_mode) {
		case M_STD:
			printf("Serial number = %d-%d\n", aquaero_data.serial_major, aquaero_data.serial_minor);
			printf("Firmware version = %d\n", aquaero_data.firmware_version);
			printf("Bootloader version = %d\n", aquaero_data.bootloader_version);
			printf("Hardware version = %d\n", aquaero_data.hardware_version);
			temp_fstr = "temp%d: %2.2f °C\n";
			fan_vrm_temp_fstr = "fan%d VRM temp: %2.2f °C\n";
			fan_current_fstr = "fan%d current: %4.2f mA\n";
			fan_rpm_fstr = "fan%d RPM: %d rpm\n";
			fan_duty_cycle_fstr = "fan%d duty cycle: %3.2f %%\n";
			fan_voltage_fstr = "fan%d voltage: %2.2f V\n";
			flow_fstr = "flow: %3.1f l/h\n";
			break;
		case M_SCRIPT:
			temp_fstr = "TEMP%d=%2.2f\n";
			fan_vrm_temp_fstr = "FAN%d_VRM_TEMP=%2.2f\n";
			fan_current_fstr = "FAN%d_CURRENT=%4.2f\n";
			fan_rpm_fstr = "FAN%d_RPM=%d\n";
			fan_duty_cycle_fstr = "FAN%d_DUTY_CYCLE=%3.2f\n";
			fan_voltage_fstr = "FAN%d_VOLTAGE=%2.2f\n";
			flow_fstr = "FLOW=%3.1f\n";
			break;
	}

	if (1) { /* print_temp */
		for (int n=0; n<AQ5_NUM_TEMP; n++)
			if (aquaero_data.temp[n] != AQ_TEMP_UNDEF)
				printf(temp_fstr, n+1, aquaero_data.temp[n]);
	}

	if (1) { /* print_fan */
		for (int n=0; n<AQ5_NUM_FAN; n++) {
			if (aquaero_data.fan_vrm_temp[n] != AQ_TEMP_UNDEF) {
				printf(fan_vrm_temp_fstr, n+1, aquaero_data.fan_vrm_temp[n]);
				printf(fan_current_fstr, n+1, aquaero_data.fan_current[n]);
				printf(fan_rpm_fstr, n+1, aquaero_data.fan_rpm[n]);
				printf(fan_duty_cycle_fstr, n+1, aquaero_data.fan_duty_cycle[n]);
				printf(fan_voltage_fstr, n+1, aquaero_data.fan_voltage[n]);
			}
		}
	}

	if (1) { /* print flow */
		printf(flow_fstr, aquaero_data.flow);
	}

	return 0;
}

/* dump_data() borrowed from original aerotools */
int dump_data(char *file, unsigned char *buffer)
{
	FILE *fh;
	
	if ((fh = fopen(file, "w")) == NULL) {
		perror(file);
		return EXIT_FAILURE;
	}
	if (fwrite(buffer, 1, AQ5_DATA_LEN, fh) != AQ5_DATA_LEN) {
		perror(file);
		fclose(fh);
		return EXIT_FAILURE;
	}

	fclose(fh);

	return EXIT_SUCCESS;
}
