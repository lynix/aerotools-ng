/* Copyright 2012-2013 lynix <lynix47@gmail.com>
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

#ifndef AEROCLI5_H_
#define AEROCLI5_H_

/* includes */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "libaquaero5.h"

/* program name */
#define PROGN                           "aerocli"

typedef struct {
	uint32_t        days;
	uint32_t        hours;
	uint32_t        minutes;
	uint32_t        seconds;
} uptime_t;


int  dump_data(char *file, unsigned char *buffer);
void get_uptime(uint32_t timeval, uptime_t *uptime);

#endif /* AEROCLI5_H_ */
