/*
  Routinen fuer Sysinfo 
  =====================
    Uwe Berger; 2015
 
  ---------
  Have fun! 

-----------------------------------------------------------------------
 Copyright (c) 2014, 2015 Uwe Berger - bergeruw(at)gmx.net
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
  
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 MA 02110-1301, USA.
 ----------------------------------------------------------------------- 
 
*/

#ifndef _SYSINFO_SCREEN_H_
#define _SYSINFO_SCREEN_H_

#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <utmp.h>

#include "framebuffer.h"
#include "globals.h"

#define SYSFS_NET_FNAME_PATTERN "/sys/class/net/%s/statistics/%s"
#define SEC_MINUTE					(60)
#define SEC_HOUR 					(SEC_MINUTE * 60)
#define SEC_DAY						(SEC_HOUR * 24)
#define ONE_MBYTE					(double)1048576		// (1024 * 1024)
#define LINUX_SYSINFO_LOADS_SCALE	(double)65536

extern char cfg_network_device[10];

// ***************************************************************
int get_number_of_users(void);
void get_rx_tx_statistic(void);
void systeminfo_output(void);

#endif
