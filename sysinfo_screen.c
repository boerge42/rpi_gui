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

#include "sysinfo_screen.h"

static double rx_kbs = 0;
static double tx_kbs = 0;


//**********************************************************************
int get_number_of_users(void)
{
	int user_count = 0;
	struct utmp *usr;

	setutent();
	while ((usr=getutent())) {
		if (usr->ut_type == USER_PROCESS) user_count++;		
	}
	return user_count;
}    

//**********************************************************************
void get_rx_tx_statistic(void)
{
	long long ms, rx, tx;
	long long d_ms, d_rx, d_tx;
	static long long old_ms=0;
	static long long old_rx=0;
	static long long old_tx=0;
	struct timeval ts;
	FILE *fp;
	char s[255];
	
	// gesendete Bytes
	sprintf(s, SYSFS_NET_FNAME_PATTERN, cfg_network_device, "tx_bytes");
	fp=fopen(s, "r");
	if (fp!=NULL) {
		fseek(fp, 0, SEEK_SET);
		fscanf(fp, "%lld", &tx);
		fclose(fp);
	} else {
		tx=0;
	}
	// empfangene Bytes
	sprintf(s, SYSFS_NET_FNAME_PATTERN, cfg_network_device, "rx_bytes");
	fp=fopen(s, "r");
	if (fp!=NULL) {
		fseek(fp, 0, SEEK_SET);
		fscanf(fp, "%lld", &rx);
		fclose(fp);
	} else {
		rx=0;
	}
	// Timestamp
	gettimeofday(&ts, NULL);
	ms = ts.tv_sec*1000+ts.tv_usec/1000;
	// noch kein Vorwert vorhanden
	if (!old_ms) {
		old_ms = ms;
		old_rx = rx;
		old_tx = tx;
		return;
	}
	// Differenzen zu Vorwerten
	d_ms = ms - old_ms;
	d_rx = rx - old_rx;
	d_tx = tx - old_tx;
	rx_kbs = (double)((d_rx/(d_ms/1000.0))/1024); 
	tx_kbs = (double)((d_tx/(d_ms/1000.0))/1024);
	old_ms=ms;
	old_rx = rx;
	old_tx = tx;
	return;
}

// ********************************************
void systeminfo_output(void)
{
	int y = 30;
	int dy = 20;
	struct sysinfo si;
	char  s[40] = "";

	sysinfo(&si);
	// Uptime
	sprintf (s, "system uptime  : %ld days, %ld:%02ld:%02ld", 
			 si.uptime / SEC_DAY, 
			(si.uptime % SEC_DAY) / SEC_HOUR, 
			(si.uptime % SEC_HOUR) / SEC_MINUTE, 
		 	 si.uptime % SEC_MINUTE);
	
	put_string(10, y, s, LIGHT_GREY, FONT_8X16);
	y+=dy;
	// Loads
	sprintf (s, "load avg.      : %4.2f %4.2f %4.2f", 
			si.loads[0] / LINUX_SYSINFO_LOADS_SCALE, 
			si.loads[1] / LINUX_SYSINFO_LOADS_SCALE, 
			si.loads[2] / LINUX_SYSINFO_LOADS_SCALE);
	put_string(10, y, s, LIGHT_GREY, FONT_8X16);
	y+=dy;
	// RAM
	sprintf (s, "ram [MByte]    : %5.1f/%5.1f", 
			(si.totalram - si.freeram) / ONE_MBYTE,
			si.totalram / ONE_MBYTE);	
	put_string(10, y, s, LIGHT_GREY, FONT_8X16);
	y+=dy;
	// SWAP
	sprintf (s, "swap [MByte]   : %5.1f/%5.1f", 
			(si.totalswap - si.freeswap) / ONE_MBYTE,
			si.totalswap / ONE_MBYTE);
	put_string(10, y, s, LIGHT_GREY, FONT_8X16);
	y+=dy;
	// Processes/User
	sprintf (s, "proc./user     : %5i/%i",
			si.procs,
			get_number_of_users());
	put_string(10, y, s, LIGHT_GREY, FONT_8X16);
	y+=dy;
	// Netzwerk Rx/Tx-Statistik
	sprintf (s, "rx/tx [kByte/s]: %5.1f/%5.1f", rx_kbs, tx_kbs);
	put_string(10, y, s, LIGHT_GREY, FONT_8X16);
}
