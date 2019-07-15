/*
 * libstatgrab
 * https://libstatgrab.org
 * Copyright (C) 2003-2004 Peter Saunders
 * Copyright (C) 2003-2019 Tim Bishop
 * Copyright (C) 2003-2013 Adam Sampson
 * Copyright (C) 2012-2019 Jens Rehsack
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#include <stdio.h>
#include <statgrab.h>
#include <stdlib.h>
#include <unistd.h>

#include "helpers.h"

static int quit;

#ifdef HAVE_LOG4CPLUS_INITIALIZE
static void *l4cplus_initializer;

static void
cleanup_logging(void)
{
	log4cplus_deinitialize(l4cplus_initializer);
}
#endif

int main(int argc, char **argv){

	extern char *optarg;
	int c;

	int delay = 1;
	sg_page_stats *page_stats;

#ifdef HAVE_LOG4CPLUS_INITIALIZE
	l4cplus_initializer = log4cplus_initialize();
	atexit((void (*)(void))cleanup_logging);
#endif

	while ((c = getopt(argc, argv, "d:")) != -1){
		switch (c){
			case 'd':
				delay = atoi(optarg);
				break;
		}
	}

	/* Initialise helper - e.g. logging, if any */
	sg_log_init("libstatgrab-examples", "SGEXAMPLES_LOG_PROPERTIES", argc ? argv[0] : NULL);

	/* Initialise statgrab */
	sg_init(1);

	register_sig_flagger( SIGINT, &quit );

	/* Drop setuid/setgid privileges. */
	if (sg_drop_privileges() != SG_ERROR_NONE)
		sg_die("Error. Failed to drop privileges", 1);

	page_stats = sg_get_page_stats_diff(NULL);
	if(page_stats == NULL)
		sg_die("Failed to get page stats", 1);

	while( (page_stats = sg_get_page_stats_diff(NULL)) != NULL){
		int ch;
		printf("Pages in : %llu\n", page_stats->pages_pagein);
		printf("Pages out : %llu\n", page_stats->pages_pageout);
		ch = inp_wait(delay);
		if( quit || (ch == 'q') )
			break;
	}

	exit(0);
}
