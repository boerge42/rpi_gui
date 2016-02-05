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

#include "weather_screen.h"

static char wdb_temperature[40] = "";
static char wdb_humidity[40]    = "";
static char wdb_pressure_rel[40]    = "";
static char wdb_timestamp[40]    = "";

typedef struct {
	char day[10];
	char date[20];
	char temp_low[10];
	char temp_high[10];
	char text[100];
	char code[10];
	char image[15];
} weather_forecast_t;

static weather_forecast_t weather_forecast[5];

static char weather_forecast_current_date[40];

static int forecast_idx = 0;
static char old_code[10] = "";

// ********************************************
int  get_forecast_idx(void)
{
	return forecast_idx;
}

// ********************************************
void incr_forecast_idx(void)
{
	if (forecast_idx < 4) {
		forecast_idx++;
		refresh_weather_screen();
	}
}

// ********************************************
void decr_forecast_idx(void)
{
	if (forecast_idx > 0) {
		forecast_idx--;
		refresh_weather_screen();
	} 
}

// ********************************************
int  refresh_weather_screen(void)
{
	return strlen(old_code);
}
	
// ********************************************
void draw_forecast_pic(void)
{
	strcpy(old_code, "");
}

// ********************************************
void weather_db_output(void)
{
	int y = 30;
	int dy = 20;
	char s[256] = "";

	// Screen partiell loeschen 
	draw_fillrect(10, 30, 318, 135, BLACK);
	draw_fillrect(62, 135, 318, 210, BLACK);
	// aktuelle Werte
	put_string(10, y, wdb_temperature, LIGHT_GREY, FONT_8X16);
	y+=dy;
	put_string(10, y, wdb_humidity,    LIGHT_GREY, FONT_8X16);
	y+=dy;
	put_string(10, y, wdb_pressure_rel,    LIGHT_GREY, FONT_8X16);
	y+=dy+5;
	put_string(10, y, wdb_timestamp,    GREY, FONT_8X8);
	y+=dy;
	y+=dy;
	// Wettervorhersage
	// Bild nur, wenn neu...
	if (strcmp(old_code, weather_forecast[forecast_idx].code)) {
		path_normalize(cfg_weather_forecast_pics, cfg_weather_forecast_pics);
//		sprintf(s, "%s%s.jpg", 
//					cfg_weather_forecast_pics,
//					weather_forecast[forecast_idx].code);
		sprintf(s, "%s%s", 
					cfg_weather_forecast_pics,
					weather_forecast[forecast_idx].image);
		jpeg_output(s, 10, y, 50, 50);
		strcpy(old_code, weather_forecast[forecast_idx].code);
		}
	sprintf(s, "%s, %s", 
				weather_forecast[forecast_idx].day,
				weather_forecast[forecast_idx].date);
	put_string(70, y , s, LIGHT_GREY, FONT_8X16);
	y+=dy;
	sprintf(s, "%s%cC / %s%cC", 
				weather_forecast[forecast_idx].temp_low, 248,
				weather_forecast[forecast_idx].temp_high, 248);
	put_string(70, y , s, LIGHT_GREY, FONT_8X16);
	y+=dy;
	sprintf(s, "%s", 
				weather_forecast[forecast_idx].text);
	put_string(70, y , s, LIGHT_GREY, FONT_8X16);
	y+=dy+5;
	put_string(70, y, weather_forecast_current_date,    GREY, FONT_8X8);
}

//**********************************************************************
void weather_db_read(void) 
{
	config_t cfg;
	const char *str, *str2;
	int	i;

	// Defaultwerte...
	sprintf(wdb_temperature,  "temp. (in/out)   : %s%cC / %s%cC", "-", 248, "-", 248);
	sprintf(wdb_humidity,     "humidity (in/out): %s%% / %s%%", "-", "-");
	sprintf(wdb_pressure_rel, "pressure (rel.)  : %shPa %s", "-", "-");
	sprintf(wdb_pressure_rel, "last update: %s", "-");
	// conf-Datei mit Wetterwerten oeffnen...
	config_init(&cfg);
	if(! config_read_file(&cfg, cfg_weather_current_values)) {
		config_destroy(&cfg);
		return;
	}
	if (config_lookup_string(&cfg, "temperature_in", &str) &&  
		config_lookup_string(&cfg, "temperature_out", &str2)) 
			sprintf(wdb_temperature, "temp. (in/out)   : %s%cC / %s%cC", str, 248, str2, 248);
	if (config_lookup_string(&cfg, "humidity_in", &str) && 
		config_lookup_string(&cfg, "humidity_out", &str2)) 
			sprintf(wdb_humidity, "humidity (in/out): %s%% / %s%%", str, str2);
	if (config_lookup_int(&cfg, "pressure_rising", &i)) {
		if (i==0) i=25; else i=24;
	} else {
		i=20;
	}
	if (config_lookup_string(&cfg, "pressure_rel", &str)) 
		sprintf(wdb_pressure_rel, "pressure (rel.)  : %shPa %c", str, i);
	if (config_lookup_string(&cfg, "timestamp", &str)) 
		sprintf(wdb_timestamp, "last update: %s", str);
	config_destroy(&cfg);
}

//**********************************************************************
int weather_forecast_read(void)
{
	config_t cfg;
	const char *str;
	char cfg_str[255] = "";
	int	i;

	// "Defaultwerte" setzen
	for (i=0; i<5; i++) {
		strncpy(weather_forecast[i].day, "-", sizeof(weather_forecast[i].day));
		strncpy(weather_forecast[i].date, "-", sizeof(weather_forecast[i].date));
		strncpy(weather_forecast[i].temp_low, "-", sizeof(weather_forecast[i].temp_low));
		strncpy(weather_forecast[i].temp_high, "-", sizeof(weather_forecast[i].temp_high));
		strncpy(weather_forecast[i].text, "-", sizeof(weather_forecast[i].text));
		strncpy(weather_forecast[i].code, "-", sizeof(weather_forecast[i].code));
		strncpy(weather_forecast[i].image, "-", sizeof(weather_forecast[i].image));
	}
	strncpy(weather_forecast_current_date, "-", sizeof(weather_forecast_current_date));
	// conf-Datei mit Vorhersagewerten oeffnen...
	config_init(&cfg);
	if(! config_read_file(&cfg, cfg_weather_forecast_values)) {
		config_destroy(&cfg);
		return 1;
	}
	// ...und auslesen
	for (i=0; i<5; i++) {
		sprintf(cfg_str, "forecast%i_day", i);
		if (config_lookup_string(&cfg, cfg_str, &str))
			strncpy(weather_forecast[i].day, str, sizeof(weather_forecast[i].day));
		sprintf(cfg_str, "forecast%i_date", i);
		if (config_lookup_string(&cfg, cfg_str, &str))
			strncpy(weather_forecast[i].date, str, sizeof(weather_forecast[i].date));
		sprintf(cfg_str, "forecast%i_temp_low", i);
		if (config_lookup_string(&cfg, cfg_str, &str)) {
			strncpy(weather_forecast[i].temp_low, str, strlen(str));
		}
		sprintf(cfg_str, "forecast%i_temp_high", i);
		if (config_lookup_string(&cfg, cfg_str, &str)) {
			strncpy(weather_forecast[i].temp_high, str, strlen(str));
		}
		sprintf(cfg_str, "forecast%i_text", i);
		if (config_lookup_string(&cfg, cfg_str, &str))
			strncpy(weather_forecast[i].text, str, sizeof(weather_forecast[i].text));
		sprintf(cfg_str, "forecast%i_code", i);
		if (config_lookup_string(&cfg, cfg_str, &str))
			strncpy(weather_forecast[i].code, str, sizeof(weather_forecast[i].code));
		sprintf(cfg_str, "forecast%i_image", i);
		if (config_lookup_string(&cfg, cfg_str, &str))
			strncpy(weather_forecast[i].image, str, sizeof(weather_forecast[i].image));
	}
	if (config_lookup_string(&cfg, "current_date", &str))
		strncpy(weather_forecast_current_date, str, sizeof(weather_forecast_current_date));
	config_destroy(&cfg);
	return 0;
}

