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

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "libaquaero5.h"

typedef enum { M_STD, M_SCRIPT } out_mode_t;

int main(int argc, char *argv[])
{
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

	/* output mode changes format strings */
	const char *temp_fstr, *fan_fstr, *flow_fstr;;

	switch (out_mode) {
		case M_STD:
			temp_fstr = "temp%d: %2.2f °C\n";
			fan_fstr = "fan%d: %d rpm\n";
			flow_fstr = "flow: %3.1f l/h\n";
			break;
		case M_SCRIPT:
			temp_fstr = "TEMP%d=%2.2f\n";
			fan_fstr = "FAN%d_RPM=%d\n";
			flow_fstr = "FLOW=%3.1f\n";
			break;
	}

	if (1) { /* print_temp */
		for (int n=0; n<AQ5_NUM_TEMP; n++)
			if (aquaero_data.temp[n] != AQ_TEMP_UNDEF)
				printf(temp_fstr, n+1, aquaero_data.temp[n]);
	}

	if (1) { /* print_fan */
		for (int n=0; n<AQ5_NUM_FAN; n++)
			printf(fan_fstr, n+1, aquaero_data.fan_rpm[n]);
	}

	if (1) { /* print flow */
		printf(flow_fstr, aquaero_data.flow);
	}

	return 0;
}
