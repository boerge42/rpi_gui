/*
  Routinen fuer Wetterinfos 
  =========================
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

#ifndef _WEATHER_SCREEN_H_
#define _WEATHER_SCREEN_H_

#include <stdio.h>
#include <stdlib.h>
#include <libconfig.h>

#include "framebuffer.h"
#include "globals.h"

extern char   cfg_weather_forecast_values[255];
extern char   cfg_weather_current_values[255];
extern char   cfg_weather_forecast_pics[255];

// ***************************************************************
void weather_db_output(void);
void weather_db_read(void);
int  weather_forecast_read(void);
void draw_forecast_pic(void);
int  refresh_weather_screen(void);
void incr_forecast_idx(void);
void decr_forecast_idx(void);
int  get_forecast_idx(void);

#endif
