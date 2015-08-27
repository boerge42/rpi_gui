/*
  
  GUI fuer meinen Rasberry PI...
  ==============================
      Uwe Berger; 2014, 2015
 
  * rudimentäre Steuerung MPD via Touch-Display
  * weitere Gimiks:
    ** Abfrage meiner Wetter-Datenbank
    ** Wettervorhersage
    ** Diashow
    ** Systeminformationen
 
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

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <libgen.h>
#include <tslib.h>
#include <libmpd/libmpd.h>
#include <libconfig.h>

#ifdef MPD_DEBUG
	#include <libmpd/debug_printf.h>
#endif

#include "framebuffer.h"
#include "globals.h"
#include "diashow_screen.h"
#include "sysinfo_screen.h"
#include "weather_screen.h"

// ein paar Systemkommandos verpacken...
// ...System anhalten/ausschalten
#define SHUTDOWN		system("sudo shutdown -Ph now")
#define REBOOT			system("sudo reboot")

// ...Display-Backlight aus-/einschalten
#define BACKLIGHT_SWITCH_OFF system("echo 1 | sudo tee /sys/class/backlight/*/bl_power 1>/dev/null")
#define BACKLIGHT_SWITCH_ON  system("echo 0 | sudo tee /sys/class/backlight/*/bl_power 1>/dev/null")

// Definitionen Bedienknoepfe
#define NR_BUTTONS 			22
#define BUTTON_SYSTEM		0
#define BUTTON_WEATHER		1
#define BUTTON_MUSIC		2
#define BUTTON_DIASHOW      3
#define BUTTON_QUEUE		4
#define BUTTON_PREV			5
#define BUTTON_PLAY			6
#define BUTTON_NEXT			7
#define BUTTON_STOP			8
#define BUTTON_REPEAT		9
#define BUTTON_RANDOM		10
#define BUTTON_PL_PREV		11
#define BUTTON_PL_NEXT		12
#define BUTTON_PL_ADD		13
#define BUTTON_PL_REPLACE	14
#define BUTTON_PL_RELOAD    15
#define BUTTON_EXIT			16
#define BUTTON_REBOOT		17
#define BUTTON_SHUTDOWN		18
#define BUTTON_DB_UPDATE	19
#define BUTTON_WEATHER_FORECAST_PREV	20
#define BUTTON_WEATHER_FORECAST_NEXT	21


#define MODE_SYSTEM		BUTTON_SYSTEM
#define MODE_WEATHER	BUTTON_WEATHER
#define MODE_MUSIC		BUTTON_MUSIC
#define MODE_DIASHOW    BUTTON_DIASHOW
#define MODE_QUEUE		BUTTON_QUEUE

#define BUTTON_SWITCH_ON(b)   buttons[b].flags |=  BUTTON_ON
#define BUTTON_SWITCH_OFF(b)  buttons[b].flags &= ~BUTTON_ON

#define BUTTON_IS_VISIBLE(b)  buttons[b].flags |=  BUTTON_VISIBLE
#define BUTTON_NOT_VISIBLE(b) buttons[b].flags &= ~BUTTON_VISIBLE

static button_t buttons[NR_BUTTONS] = {
// "feste" Button	
{276, 219, 43, 20, "Sys.",    WHITE, BLUE,  YELLOW, FONT_8X16, WHITE, YELLOW, BLUE, WHITE, RED, YELLOW, 2},	
{0  , 219, 66, 20, "Weather", WHITE, BLUE,  YELLOW, FONT_8X16, WHITE, YELLOW, BLUE, WHITE, RED, YELLOW, 2},	
{69 , 219, 66, 20, "Music",   WHITE, BLUE,  YELLOW, FONT_8X16, WHITE, YELLOW, BLUE, WHITE, RED, YELLOW, 2},		
{138, 219, 66, 20, "Diashow", WHITE, BLUE,  YELLOW, FONT_8X16, WHITE, YELLOW, BLUE, WHITE, RED, YELLOW, 2},		
{207, 219, 66, 20, "Queue",   WHITE, BLUE,  YELLOW, FONT_8X16, WHITE, YELLOW, BLUE, WHITE, RED, YELLOW, 2},
// Screen "Music"
{5 , 190, 49, 20,  "Prev",   WHITE, GREEN, WHITE,  FONT_8X16, WHITE, YELLOW, BLUE, WHITE, RED, YELLOW, 0},
{57 , 190, 49, 20, "Play",   WHITE, GREEN, WHITE,  FONT_8X16, WHITE, YELLOW, BLUE, WHITE, RED, YELLOW, 0},
{109, 190, 49, 20, "Next",   WHITE, GREEN, WHITE,  FONT_8X16, WHITE, YELLOW, BLUE, WHITE, RED, YELLOW, 0},
{161, 190, 49, 20, "Stop",   WHITE, GREEN, WHITE,  FONT_8X16, WHITE, YELLOW, BLUE, WHITE, RED, YELLOW, 0},
{213, 190, 49, 20, "Repeat", WHITE, GREEN, WHITE,  FONT_8X16, WHITE, YELLOW, BLUE, WHITE, RED, YELLOW, 0},
{265, 190, 49, 20, "Random", WHITE, GREEN, WHITE,  FONT_8X16, WHITE, YELLOW, BLUE, WHITE, RED, YELLOW, 0},
// Screen "Queue"
{  5, 55, 74, 20, "Prev",    WHITE, GREEN, WHITE,  FONT_8X16, WHITE, YELLOW, BLUE, WHITE, RED, YELLOW, 0},
{239, 55, 74, 20, "Next",    WHITE, GREEN, WHITE,  FONT_8X16, WHITE, YELLOW, BLUE, WHITE, RED, YELLOW, 0},
{ 83, 55, 74, 20, "Add",     WHITE, GREEN, WHITE,  FONT_8X16, WHITE, YELLOW, BLUE, WHITE, RED, YELLOW, 0},
{161, 55, 74, 20, "Replace", WHITE, GREEN, WHITE,  FONT_8X16, WHITE, YELLOW, BLUE, WHITE, RED, YELLOW, 0},
{  5, 79, 308, 20, "Reload playlists...", WHITE, GREEN, WHITE,  FONT_8X16, WHITE, YELLOW, BLUE, WHITE, RED, YELLOW, 0},
// Screen "System"
{  5, 190, 101, 20, "Exit",    WHITE, GREEN, WHITE,  FONT_8X16, WHITE, YELLOW, BLUE, WHITE, RED, YELLOW, 0},
{109, 190, 101, 20, "Reboot",  WHITE, GREEN, WHITE,  FONT_8X16, WHITE, YELLOW, BLUE, WHITE, RED, YELLOW, 0},
{213, 190, 101, 20, "Shutdown",WHITE, GREEN, WHITE,  FONT_8X16, WHITE, YELLOW, BLUE, WHITE, RED, YELLOW, 0},
// Screen "Queue"
{  5, 155, 308, 20, "MPD-DB update...", WHITE, GREEN, WHITE,  FONT_8X16, WHITE, YELLOW, BLUE, WHITE, RED, YELLOW, 0},
// Screen "Weather (Forecast)"
{  211, 135, 51, 20, "Prev", WHITE, GREEN, WHITE,  FONT_8X16, WHITE, YELLOW, BLUE, WHITE, RED, YELLOW, 0},
{  265, 135, 51, 20, "Next", WHITE, GREEN, WHITE,  FONT_8X16, WHITE, YELLOW, BLUE, WHITE, RED, YELLOW, 0}
};

// Backlight
unsigned int backlight_timer;
unsigned char backlight_on = 1;

// Diashow-Timer
unsigned int diashow_timer;

// xy-Ausdehnung Framebuffer
static int  xres, yres;

// Zeugs fuer MPD etc.
#define MAX_LEN_STRING	37
static MpdObj *mpd = NULL;
static char mpd_artist[MAX_LEN_STRING] = "";
static char mpd_album[MAX_LEN_STRING] = "";
static char mpd_title[MAX_LEN_STRING] = "";
static char mpd_track[MAX_LEN_STRING] = "";
static char mpd_name[MAX_LEN_STRING] = "";
static char mpd_comment[MAX_LEN_STRING] = "";
static char mpd_file[PATH_MAX+1] = "";
static char cd_cover_file[PATH_MAX+1] = "";
static int  draw_new_cd_cover = 0;
static char mpd_total_time_str[10] = "";
static char mpd_elapsed_time_str[10] = "";
static char mpd_playlist_str[MAX_LEN_STRING] = "";
static int  mpd_total_time_int = 0;
static int  mpd_elapsed_time_int = 0;
static int  mpd_playlist_len_int = 0;
static int  mpd_playlist_pos_int = 0;
static char mpd_playlist_pos_str[10] = "";
static char mpd_playlist_len_str[10] = "";
static char mpd_format_str[50] = "No formatinfo!";
static int	mpd_status_play = 0;
static int  mpd_no_status_update = 0;

// Zeugs fuer Playlist-Anzeige
struct playlist_struct {
	char name[30];
	struct playlist_struct* prev;
	struct playlist_struct* next;
};

struct playlist_struct* playlists = NULL;

// fuer Zeit, Datum etc.
time_t now;
struct tm *now_tm;
char date_time[21] = "";

// aktueller Anzeige-Mode/-Bildschirm
int mode = MODE_WEATHER;

// Variabeln aus/fuer Konfigurationsdatei (rpi_gui.conf)
// (...mit kryptischen Vorbelegungen ;-)) 
#define CONFIG_FILE	"/etc/rpi_gui.conf"
int cfg_backlight_time=60;
int cfg_default_screen=1;
char cfg_network_device[10]="wlan0";
char cfg_diashow_path[255]=
	"/media/fritzbox/WD-MyPassport0810-01/bilder/diashow";
char cfg_diashow_file_pattern[255]="*.jpg";
int cfg_diashow_photo_time=10;
char cfg_mpd_path[255]=
	"/media/fritzbox/WD-MyPassport0810-01/mp3";
char cfg_cd_cover_fname[255]=
	"cd_cover_128x128.jpg";
char cfg_weather_current_values[255]=
	"weather_current.conf";
char cfg_weather_forecast_values[255]=
	"weather_forecast.conf";
char cfg_weather_forecast_pics[255]=
	"";
char cfg_mpd_hostname[255] = "localhost";
int cfg_mpd_port = 6600;
char cfg_mpd_password[255] = "";

// forwards
void mpd_init(void);
void mpd_error_callback(MpdObj *mi,int errorid, char *msg, void *userdata);
void mpd_status_changed(MpdObj *mi, ChangedStatusType what);
void mpd_lsplaylists(MpdObj *mi);


//**********************************************************************
int read_config(void)
{
	config_t cfg;
	const char *str;
	int	i;

	// Konfiguration oeffnen...
	config_init(&cfg);
	if(! config_read_file(&cfg, CONFIG_FILE)) {
		config_destroy(&cfg);
		return(1);
	}
	// ...und einlesen
	if (config_lookup_int(&cfg, "cfg_backlight_time", &i))
		cfg_backlight_time=i;
	if (config_lookup_int(&cfg, "cfg_default_screen", &i))
		cfg_default_screen=i;
	if (config_lookup_string(&cfg, "cfg_network_device", &str))
		strncpy(cfg_network_device, str, sizeof(cfg_network_device));
	if (config_lookup_string(&cfg, "cfg_weather_forecast_values", &str))
		strncpy(cfg_weather_forecast_values, str, sizeof(cfg_weather_forecast_values));
	if (config_lookup_string(&cfg, "cfg_weather_current_values", &str))
		strncpy(cfg_weather_current_values, str, sizeof(cfg_weather_current_values));
	if (config_lookup_string(&cfg, "cfg_weather_forecast_pics", &str))
		strncpy(cfg_weather_forecast_pics, str, sizeof(cfg_weather_forecast_pics));
	if (config_lookup_string(&cfg, "cfg_diashow_path", &str))
		strncpy(cfg_diashow_path, str, sizeof(cfg_diashow_path));
	if (config_lookup_string(&cfg, "cfg_diashow_file_pattern", &str))
		strncpy(cfg_diashow_file_pattern, str, sizeof(cfg_diashow_file_pattern));
	if (config_lookup_int(&cfg, "cfg_diashow_photo_time", &i))
		cfg_diashow_photo_time=i;
	if (config_lookup_string(&cfg, "cfg_mpd_path", &str))
		strncpy(cfg_mpd_path, str, sizeof(cfg_mpd_path));
	if (config_lookup_string(&cfg, "cfg_cd_cover_fname", &str))
		strncpy(cfg_cd_cover_fname, str, sizeof(cfg_cd_cover_fname));
	if (config_lookup_string(&cfg, "cfg_mpd_hostname", &str))
		strncpy(cfg_mpd_hostname, str, sizeof(cfg_mpd_hostname));
	if (config_lookup_int(&cfg, "cfg_mpd_port", &i))
		cfg_mpd_port=i;
	if (config_lookup_string(&cfg, "cfg_mpd_password", &str))
		strncpy(cfg_mpd_password, str, sizeof(cfg_mpd_password));
	config_destroy(&cfg);
	return 0;
}

//**********************************************************************
int cd_cover_exist(void)
{
	return (strlen(cd_cover_file) != 0);
}

//**********************************************************************
void mpd_init(void)
{
	mpd = mpd_new(cfg_mpd_hostname, cfg_mpd_port, cfg_mpd_password);
	mpd_signal_connect_error(mpd,(ErrorCallback)mpd_error_callback, NULL);
    mpd_signal_connect_status_changed(mpd,(StatusChangedCallback)mpd_status_changed, NULL);
	mpd_set_connection_timeout(mpd, 10);
	mpd_connect(mpd);
}

//**********************************************************************
void mpd_lsplaylists(MpdObj *mi) 
{
	MpdData *data;
	struct playlist_struct* ptr;
	struct playlist_struct* prev;
	
	mpd_no_status_update=1;
	
	// eventuell alte vorhandene Liste mit Playlisten loeschen
	if (playlists != NULL) {
		// an Anfang der Liste...
		while (playlists->prev!=NULL) playlists=playlists->prev;
		// ...und bis zum Ende loeschen
		while (playlists != NULL) {
			ptr = playlists->next;
			free(playlists);
			playlists = ptr;	
		}
	}	
	// Liste mit Playlisten erzeugen...
	for (data=mpd_database_playlist_list(mi); data!=NULL; data=mpd_data_get_next(data)){
		if (playlists == NULL) {
			playlists = malloc(sizeof(*playlists));	
			strcpy(playlists->name, data->playlist->path);
			playlists->prev = NULL;
			playlists->next = NULL;
		} else {
			ptr = playlists;
			while (ptr->next != NULL) ptr = ptr->next;
			prev = ptr;
			ptr->next = malloc(sizeof(*ptr));	
			ptr=ptr->next;
			strcpy(ptr->name, data->playlist->path);
			ptr->prev = prev;
			ptr->next = NULL;
		}	
	}
	
	mpd_no_status_update=0;
}

//**********************************************************************
void mpd_error_callback(MpdObj *mi,int errorid, char *msg, void *userdata)
{
	switch (errorid) {
		case 10:
			mpd_init();
			break;
	}
} 

//**********************************************************************
void mpd_status_changed(MpdObj *mi, ChangedStatusType what)
{
	int path_len; 
	
	// aktuelle Playliste hat sich geaendert
	if(what&MPD_CST_PLAYLIST) {
		mpd_playlist_len_int = mpd_playlist_get_playlist_length(mi);
		sprintf(mpd_playlist_str, "Position: %i/%i", mpd_playlist_pos_int, mpd_playlist_len_int);
		sprintf(mpd_playlist_len_str, "%i", mpd_playlist_len_int);
	}
	// aktueller Song hat sich geaendert
	if((what&MPD_CST_SONGID)||(what&MPD_CST_PLAYLIST)) {
		mpd_Song *song = mpd_playlist_get_current_song(mi);
		if(song) {
			mpd_song_markup(mpd_artist, sizeof(mpd_artist), "%artist%", song);
			mpd_song_markup(mpd_album, sizeof(mpd_album), "%album%", song);
			mpd_song_markup(mpd_title, sizeof(mpd_title), "%title%", song);
			mpd_song_markup(mpd_track, sizeof(mpd_track), "%track%", song);
			mpd_song_markup(mpd_name, sizeof(mpd_name), "%name%", song);
			mpd_song_markup(mpd_comment, sizeof(mpd_comment), "%comment%", song);
			mpd_song_markup(mpd_file, sizeof(mpd_file), "%file%", song);
			mpd_playlist_pos_int = song->pos + 1;
			sprintf(mpd_playlist_str, "Position: %i/%i", mpd_playlist_pos_int, mpd_playlist_len_int);
			sprintf(mpd_playlist_pos_str, "%i", mpd_playlist_pos_int);
			// Pfad zur CD-Cover-Datei zusammenbasteln
			// ...Pfad zu den MP3-Dateien (generell) 
			strncpy (cd_cover_file, cfg_mpd_path, sizeof (cd_cover_file));
			path_normalize(cd_cover_file, cd_cover_file);
			// ...plus Pfad zu aktueller Musik-Datei 
			path_len = strlen(cd_cover_file);
			strncpy (cd_cover_file + path_len, dirname(mpd_file), sizeof(cd_cover_file)-path_len);
			path_normalize(cd_cover_file, cd_cover_file);
			// ...plus Dateiname der CD-Cover-Bilddatei
			path_len = strlen(cd_cover_file);
			strncpy (cd_cover_file + path_len, cfg_cd_cover_fname, sizeof(cd_cover_file)-path_len);
			// ...CD-Cover_Bilddatei vorhanden?
			if (!file_exist(cd_cover_file)) {
				// ...auch nicht mit Underlines statt Leerzeichen?
				filename_with_underlines(cd_cover_file, cd_cover_file);
				if (!file_exist(cd_cover_file)) {
					strncpy(cd_cover_file, "", 1);
				} 
			} 
			
			draw_new_cd_cover = 1;
		}
	}
	// Abspielstatus hat sich geaendert
	if(what&MPD_CST_STATE) {
		switch(mpd_player_get_state(mi)) {
			case MPD_PLAYER_PLAY:
				BUTTON_SWITCH_ON(BUTTON_PLAY);
				mpd_status_play = 1;
				backlight_timer = cfg_backlight_time;
				if (!backlight_on) {
					backlight_on = 1;
					BACKLIGHT_SWITCH_ON;
				}
				if (mode != MODE_QUEUE) mode = MODE_MUSIC;
				break;
			case MPD_PLAYER_PAUSE:
			case MPD_PLAYER_STOP:
				BUTTON_SWITCH_OFF(BUTTON_PLAY);
				mpd_status_play = 0;
				break;
			default:
				break;
		}
	}
	// Gesamtabspielzeit des aktuellen Songs hat sich geaendert
	if(what&MPD_CST_TOTAL_TIME){
		mpd_total_time_int = mpd_status_get_total_song_time(mi);
		sprintf(mpd_total_time_str, "%02i:%02i", 
				mpd_total_time_int/60, mpd_total_time_int%60);
	}
	// vergangene Abspielzeit des aktuellen Songs hat sich geaendert
	if(what&MPD_CST_ELAPSED_TIME){
		mpd_elapsed_time_int = mpd_status_get_elapsed_song_time(mi);
		sprintf(mpd_elapsed_time_str, "%02i:%02i", 
				mpd_elapsed_time_int/60, mpd_elapsed_time_int%60);
	}
	// Wiederholung?
	if(what&MPD_CST_REPEAT){
		if (mpd_player_get_repeat(mi)) {
			BUTTON_SWITCH_ON(BUTTON_REPEAT);
		} else {
			BUTTON_SWITCH_OFF(BUTTON_REPEAT);
		}
	}
	// Zufallswiedergabe?
	if(what&MPD_CST_RANDOM){
		if (mpd_player_get_random(mi)) {
			BUTTON_SWITCH_ON(BUTTON_RANDOM);
		} else {
			BUTTON_SWITCH_OFF(BUTTON_RANDOM);
		}
	}
	// Update-Status DB
	if(what&MPD_CST_UPDATING) {
		if(mpd_status_db_is_updating(mi)) {
			//printf("Started updating DB\n");
		} else {
			//printf("Updating DB finished\n");
		}
	}
	// DB geaendert?
	if(what&MPD_CST_DATABASE){
		//printf("Databased changed\n");
	}
	// Playlist geaendert
	if(what&MPD_CST_PLAYLIST){
		//printf("Playlist changed\n");
	}
	// ein paar Informationen zum Format der (aktuellen) Musikdatei holen
	sprintf(mpd_format_str, "%i Cannels/%ibps/%ikbs/%iHz",
				mpd_status_get_channels(mi),
				mpd_status_get_bits(mi),				
				mpd_status_get_bitrate(mi),
				mpd_status_get_samplerate(mi));
}

// ********************************************
void mpd_music_output(void)
{
	int y = 20;
	int dy = 20;	
	int dy1 = 10;
	int x1, x2, dx;
	int no_info = 1;

	if (cd_cover_exist()) {
		// Ausgabe mit CD-Cover...
		if (draw_new_cd_cover) {
			draw_fillrect(2, 10, 130, 138, BLACK);
			jpeg_output(cd_cover_file, 2, 10, 128, 128);
			draw_new_cd_cover = 0;
		}
		// restliche Song-Daten ausgeben...
		// ...alte Daten vom Screen loeschen
		draw_fillrect(131, 16, 318, 189, BLACK);
		draw_fillrect(2, 139, 318, 189, BLACK);
		
		// ...Artist
		y=25;
		put_string(135, y, "Artist:", GREY, FONT_8X8);
		y+=dy1;
		put_string(135, y, mpd_artist, LIGHT_GREY, FONT_8X16);

		// ...Album
		y=60;
		put_string(135, y, "Album:", GREY, FONT_8X8);
		y+=dy1;
		put_string(135, y, mpd_album, LIGHT_GREY, FONT_8X16);

		y=95;
		put_string(135, y, "Title:", GREY, FONT_8X8);
		y+=dy1;
		put_string(135, y, mpd_title, LIGHT_GREY, FONT_8X16);

		// ...Zeitbalken
		if (mpd_total_time_int) {
			y = 146;
			put_string(10, y, mpd_elapsed_time_str, LIGHT_GREY, FONT_8X8);
			put_string(xres-10-8*5, y, mpd_total_time_str, LIGHT_GREY, FONT_8X8);
			x1=10+8*5+5;
			x2=xres-10-8*5-5;
			dx=x2-x1;
			draw_rect(x1, y, x1+dx, y+8, LIGHT_GREY);
			if (mpd_total_time_int) dx=dx*mpd_elapsed_time_int/mpd_total_time_int; else dx=0;
			draw_fillrect(x1, y, x1+dx, y+8, LIGHT_GREY);
			y+=dy;
			no_info=0;
		}
		// ...Song-Format
		y=162;
		put_string(10, y, mpd_format_str, LIGHT_GREY, FONT_8X8);
		// ...Position in Queue
		y=177;
		put_string(10, y, mpd_playlist_str, LIGHT_GREY, FONT_8X8);
		
	} else {
		// ohne CD-Cover...
		if (strlen(mpd_artist)) {
			put_string(10, y, "Artist:", GREY, FONT_8X8);
			y+=dy1;
			put_string(10, y, mpd_artist, LIGHT_GREY, FONT_8X16);
			y+=dy;
			no_info=0;
		} else {
			if (strlen(mpd_name)) {
				put_string(10, y, "Name:", GREY, FONT_8X8);
				y+=dy1;
				put_string(10, y, mpd_name, LIGHT_GREY, FONT_8X16);
				y+=dy;		
				no_info=0;
			}
		}
			
		if (strlen(mpd_album)) {
			put_string(10, y, "Album:", GREY, FONT_8X8);
			y+=dy1;
			put_string(10, y, mpd_album, LIGHT_GREY, FONT_8X16);
			y+=dy;
			no_info=0;
		}
	
		if (strlen(mpd_title)) {
			put_string(10, y, "Title:", GREY, FONT_8X8);
			y+=dy1;
			put_string(10, y, mpd_title, LIGHT_GREY, FONT_8X16);
			y+=dy;
			no_info=0;
		}
	
		// Zeitbalken...
		if (mpd_total_time_int) {
			put_string(10, y, "Songtime:", GREY, FONT_8X8);
			y+=dy1;
			put_string(10, y, mpd_elapsed_time_str, LIGHT_GREY, FONT_8X16);
			put_string(xres-10-8*5, y, mpd_total_time_str, LIGHT_GREY, FONT_8X16);
			x1=10+8*5+5;
			x2=xres-10-8*5-5;
			dx=x2-x1;
			draw_rect(x1, y, x1+dx, y+16, LIGHT_GREY);
			if (mpd_total_time_int) dx=dx*mpd_elapsed_time_int/mpd_total_time_int; else dx=0;
			draw_fillrect(x1, y, x1+dx, y+16, LIGHT_GREY);
			y+=dy;
			no_info=0;
		}
	

		if (no_info) put_string_center(160, 80, "No Info!", LIGHT_GREY, FONT_8X16);

		if (!no_info) {
			y=145;
			put_string(10, y, mpd_format_str, LIGHT_GREY, FONT_8X8);
		}

		y=160;
		put_string(10, y, "Current queue:", GREY, FONT_8X8);
		y+=dy1;
		put_string(10, y, mpd_playlist_str, LIGHT_GREY, FONT_8X16);
	}
}

// ********************************************
void mpd_queue_output(void)
{
	int y = 25;
	time_t rawtime;
	struct tm * timeinfo;
	char buf[40];
	
	if (playlists != NULL) {
		put_string(10, y, "Playlists:", GREY, FONT_8X8);
		y+=10;
		put_string(10, y, playlists->name, LIGHT_GREY, FONT_8X16);
	} else {
		put_string_center(160, 70, "No playlists found!", LIGHT_GREY, FONT_8X16);
	}
	
	y=120;
	put_string(10, y, "MPD database:", GREY, FONT_8X8);
	y+=10;
	
	rawtime = mpd_server_get_database_update_time(mpd);
	timeinfo = localtime (&rawtime);
	strftime (buf, 40, "last update: %d.%m.%Y; %T", timeinfo);
	put_string(10, y, buf, LIGHT_GREY, FONT_8X16);
	
}

// ********************************************
static void sig(int sig)
{
	close_framebuffer();
	fflush(stderr);
	fflush(stdout);
	BACKLIGHT_SWITCH_ON;
	exit(1);
}

// ********************************************
void refresh_screen(void)
{
	int i;
	
	// alle Button unsichtbar
	for (i=0; i<NR_BUTTONS; i++) BUTTON_NOT_VISIBLE(i);
	
	// wenn Diashow, dann nur (eventuell) neues Bild und wieder raus
	if (mode==MODE_DIASHOW) {
		if (!(diashow_timer)) {
			next_photo();
			draw_new_cd_cover = 1;
			diashow_timer=cfg_diashow_photo_time;
		}
		draw_forecast_pic();
	} else {
		// nicht immer den ganzen Bildschirm loeschen...
		if ((!cd_cover_exist()||(mode!=MODE_MUSIC))&&(!refresh_weather_screen()||mode!=MODE_WEATHER)) 
			clear_screen();
		// Rahmen
		draw_rect(0, font[FONT_8X16].height/2, xres-1, 215, WHITE);
		// Datum/Uhrzeit
		draw_fillrect(xres-(strlen(date_time)+1)*font[FONT_8X16].width-2, 0,
					xres-font[FONT_8X16].width, font[FONT_8X16].height,
					BLACK);
		put_string(xres-(strlen(date_time)+1)*font[FONT_8X16].width, 0, date_time, WHITE, FONT_8X16);
		// "feste" Button sichtbar
		BUTTON_IS_VISIBLE(BUTTON_WEATHER);
		BUTTON_IS_VISIBLE(BUTTON_MUSIC);
		BUTTON_IS_VISIBLE(BUTTON_DIASHOW);
		BUTTON_IS_VISIBLE(BUTTON_QUEUE);
		BUTTON_IS_VISIBLE(BUTTON_SYSTEM);
		// Status "fester" Button zuruecksetzen
		BUTTON_SWITCH_OFF(BUTTON_WEATHER);
		BUTTON_SWITCH_OFF(BUTTON_MUSIC);
		BUTTON_SWITCH_OFF(BUTTON_DIASHOW);
		BUTTON_SWITCH_OFF(BUTTON_QUEUE);
		BUTTON_SWITCH_OFF(BUTTON_SYSTEM);
		// alle "variablen" Button unsichtbar
		BUTTON_NOT_VISIBLE(BUTTON_PLAY);
		BUTTON_NOT_VISIBLE(BUTTON_PREV);
		BUTTON_NOT_VISIBLE(BUTTON_NEXT);
		BUTTON_NOT_VISIBLE(BUTTON_STOP);
		BUTTON_NOT_VISIBLE(BUTTON_REPEAT);
		BUTTON_NOT_VISIBLE(BUTTON_RANDOM);
		BUTTON_NOT_VISIBLE(BUTTON_PL_PREV);
		BUTTON_NOT_VISIBLE(BUTTON_PL_NEXT);
		BUTTON_NOT_VISIBLE(BUTTON_PL_ADD);
		BUTTON_NOT_VISIBLE(BUTTON_PL_REPLACE);
		BUTTON_NOT_VISIBLE(BUTTON_PL_RELOAD);
		BUTTON_NOT_VISIBLE(BUTTON_EXIT);
		BUTTON_NOT_VISIBLE(BUTTON_REBOOT);
		BUTTON_NOT_VISIBLE(BUTTON_SHUTDOWN);	
		BUTTON_NOT_VISIBLE(BUTTON_DB_UPDATE);	
		BUTTON_NOT_VISIBLE(BUTTON_WEATHER_FORECAST_PREV);	
		BUTTON_NOT_VISIBLE(BUTTON_WEATHER_FORECAST_NEXT);			
		// Content
		switch (mode) {
			case MODE_SYSTEM:
				systeminfo_output();
				draw_new_cd_cover = 1;	
				draw_forecast_pic();
				BUTTON_SWITCH_ON(BUTTON_SYSTEM);
				BUTTON_IS_VISIBLE(BUTTON_EXIT);
				BUTTON_IS_VISIBLE(BUTTON_REBOOT);
				BUTTON_IS_VISIBLE(BUTTON_SHUTDOWN);
				break;
			case MODE_WEATHER:
				weather_db_output();	
				draw_new_cd_cover = 1;
				BUTTON_SWITCH_ON(BUTTON_WEATHER);
				if (get_forecast_idx() > 0) BUTTON_IS_VISIBLE(BUTTON_WEATHER_FORECAST_PREV);
				if (get_forecast_idx() < 3) BUTTON_IS_VISIBLE(BUTTON_WEATHER_FORECAST_NEXT);
				break;
			case MODE_MUSIC:
				mpd_music_output();
				draw_forecast_pic();
				BUTTON_SWITCH_ON(BUTTON_MUSIC);
				BUTTON_IS_VISIBLE(BUTTON_PLAY);
				BUTTON_IS_VISIBLE(BUTTON_PREV);
				BUTTON_IS_VISIBLE(BUTTON_NEXT);
				BUTTON_IS_VISIBLE(BUTTON_STOP);
				BUTTON_IS_VISIBLE(BUTTON_REPEAT);
				BUTTON_IS_VISIBLE(BUTTON_RANDOM);
				break;
			case MODE_QUEUE:
				mpd_queue_output();
				draw_new_cd_cover = 1;
				draw_forecast_pic();
				if (playlists != NULL) {
					BUTTON_IS_VISIBLE(BUTTON_PL_ADD);
					BUTTON_IS_VISIBLE(BUTTON_PL_REPLACE);
					if (playlists->prev != NULL) {
						BUTTON_IS_VISIBLE(BUTTON_PL_PREV);
					}
					if (playlists->next != NULL) {
						BUTTON_IS_VISIBLE(BUTTON_PL_NEXT);
					}
				}
				BUTTON_IS_VISIBLE(BUTTON_PL_RELOAD);
				if (mpd_status_db_is_updating(mpd)) {
					BUTTON_SWITCH_ON(BUTTON_DB_UPDATE);
				} else {
					BUTTON_SWITCH_OFF(BUTTON_DB_UPDATE);
				}
				BUTTON_IS_VISIBLE(BUTTON_DB_UPDATE);
				BUTTON_SWITCH_ON(BUTTON_QUEUE);
				break;
			default:
				break;	
			}				
		// Button malen...
		for (i=0; i<NR_BUTTONS; i++) draw_button(&buttons[i]);
	}
}

// ********************************************
void sec_beat(int signal) {
	// Uhrzeit
	now = time (NULL);
	now_tm	= localtime (&now);
	sprintf(date_time, "%02d.%02d.%04d; %02d:%02d:%02d", 
			now_tm->tm_mday, now_tm->tm_mon+1, now_tm->tm_year+1900,
			now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec);

	// jede 5.Sekunden Statistik Netzwerkschnittstelle ermitteln
	if ((now_tm->tm_sec%5)==0) 
		get_rx_tx_statistic();
			
	// jede 2.Minute Wetterdaten holen
	if ((now_tm->tm_sec==15)&&((now_tm->tm_min%3)==0)&&(mode==MODE_WEATHER)) 
		weather_db_read();
	
	// jede Stunde Wettervorhersage einlesen
	if ((now_tm->tm_sec==10)&&(now_tm->tm_min==15))
		weather_forecast_read();
	
	// wenn keine Playlisten, dann mal neu lesen...
	if ((playlists == NULL)&&((now_tm->tm_sec%30)==0)) mpd_lsplaylists(mpd);
	
	// MPD-Status-Abfrage anstossen
	if (!mpd_no_status_update) mpd_status_update(mpd);
	
	// Bildschirm neu aufbauen
	refresh_screen();
	
	// wenn Diashow, dann:
	// ...keine Backlight-Steuerung 
	// ..."Bildwechsel-Zeit" steuern
	// ...raus
	if (mode==MODE_DIASHOW) {
		backlight_timer=cfg_backlight_time;
		diashow_timer--;
		alarm(1);
		return;
	}
	
	// Display-Backlight steuern...
	if (backlight_timer) {
		if (!mpd_status_play) backlight_timer--;	
	} else {
		if (backlight_on) {
			backlight_on = 0;
			BACKLIGHT_SWITCH_OFF;
			mode = MODE_WEATHER;
		}
	}

	// ...in einer Sekunde wieder...
	alarm(1);
} 


// ********************************************
// ********************************************
// ********************************************
int main(void)
{

	struct tsdev *ts;
	char *tsdevice=NULL;
	int i;

	signal(SIGSEGV, sig);
	signal(SIGINT,  sig);
	signal(SIGTERM, sig);
	
	// rpi_gui.conf einlesen
	read_config();
	backlight_timer = cfg_backlight_time;
	diashow_timer = cfg_diashow_photo_time;

	// Wetterdaten das erste Mal sammeln
	weather_db_read();
	weather_forecast_read();

	// Display-Backlight anschalten
	BACKLIGHT_SWITCH_ON;
	
	// Framebuffer
	if (open_framebuffer(&xres,&yres)) {
		exit(1);	
	}

	// Touchsreen	
	if( (tsdevice = getenv("TSLIB_TSDEVICE")) != NULL ) {
		ts = ts_open(tsdevice,0);
	} else {
		if (!(ts = ts_open("/dev/input/touchscreen", 0)))
			ts = ts_open("/dev/touchscreen/ucb1x00", 0);
	}
	if (!ts) {
		perror("ts_open");
		exit(1);
	}
	if (ts_config(ts)) {
		perror("ts_config");
		exit(1);
	}	
		
#ifdef MPD_DEBUG
	debug_set_level(DEBUG_WARNING);
#endif

	// Verbindung zum MPD-Dienst oeffnen/Playlisten laden
	mpd_init();
	mpd_lsplaylists(mpd);
	
	// ...die erste Anzeige...
	refresh_screen();
	
	// Sekundentimer initialisieren (fast so, wie bei MCs :-))
	signal(SIGALRM, sec_beat);
	alarm(1); 

	// Endlosschleife...
	while (1) {
		struct ts_sample samp;
		int ret;
		
		// Touchscreen?
		ret = ts_read(ts, &samp, 1);
		if (ret < 0) {
			perror("ts_read");
			exit(1);
		}
		if (ret != 1) continue;
		
		backlight_timer = cfg_backlight_time;
		if (!backlight_on) {
			// wenn Backlight aus --> wieder anschalten, wenn Touch
			backlight_on = 1;
			BACKLIGHT_SWITCH_ON;
		} else {
			// koennte auch Diashow sein..., dann Wetterscreen...
			if (mode==MODE_DIASHOW) mode=MODE_WEATHER;
			
			// ansonsten schauen, welcher Button	
			for (i = 0; i < NR_BUTTONS; i++)
				if (button_handle(&buttons[i], &samp)) {
					// welcher Button wurde beruehrt
					switch (i) {
					case BUTTON_EXIT:
						mpd_free(mpd);
						close_framebuffer();
						exit(0);
						break;
					case BUTTON_REBOOT:
						mpd_free(mpd);
						close_framebuffer();
						REBOOT;
						exit(0);
						break;
					case BUTTON_SHUTDOWN:
						mpd_free(mpd);
						close_framebuffer();
						SHUTDOWN;
						exit(0);
						break;
					case BUTTON_SYSTEM:
						mode = MODE_SYSTEM;
						refresh_screen();
						break;
					case BUTTON_WEATHER:
						mode = MODE_WEATHER;
						refresh_screen();
						break;
					case BUTTON_MUSIC:	
						mode = MODE_MUSIC;
						clear_screen();
						refresh_screen();
						break;
					case BUTTON_DIASHOW:	
						mode = MODE_DIASHOW;
						next_photo();
						refresh_screen();
						break;
					case BUTTON_QUEUE:	
						if (mode != MODE_QUEUE) {
							mode = MODE_QUEUE;
						}
						refresh_screen();
						break;
					case BUTTON_PLAY:
						switch(mpd_player_get_state(mpd)) {
							case MPD_PLAYER_PLAY:
								mpd_player_pause(mpd);
								break;
							case MPD_PLAYER_PAUSE:
							case MPD_PLAYER_STOP:
								mpd_player_play(mpd);
								break;
						}
						break;
					case BUTTON_PREV:
						mpd_player_prev(mpd);	
						break;
					case BUTTON_NEXT:
						mpd_player_next(mpd);	
						break;
					case BUTTON_STOP:	
						mpd_player_stop(mpd);
						break;
					case BUTTON_REPEAT:	
						mpd_player_set_repeat(mpd, !mpd_player_get_repeat(mpd));
						break;
					case BUTTON_RANDOM:	
						mpd_player_set_random(mpd, !mpd_player_get_random(mpd));
						break;
					case BUTTON_PL_PREV:
						if (playlists->prev!=NULL) playlists=playlists->prev;
						refresh_screen();	
						break;
					case BUTTON_PL_NEXT:	
						if (playlists->next!=NULL) playlists=playlists->next;
						refresh_screen();	
						break;
					case BUTTON_PL_ADD:	
						mpd_playlist_queue_load(mpd, playlists->name);
						mpd_playlist_queue_commit(mpd);
						switch(mpd_player_get_state(mpd)) {
							case MPD_PLAYER_PAUSE:
							case MPD_PLAYER_STOP:
								mpd_player_play(mpd);
								break;
						}
						break;
					case BUTTON_PL_REPLACE:	
						mpd_playlist_clear(mpd);
						mpd_playlist_queue_load(mpd, playlists->name);
						mpd_playlist_queue_commit(mpd);
						switch(mpd_player_get_state(mpd)) {
							case MPD_PLAYER_PAUSE:
							case MPD_PLAYER_STOP:
								mpd_player_play(mpd);
								break;
						}
						break;
					case BUTTON_PL_RELOAD:	
						mpd_lsplaylists(mpd);
						break;
					case BUTTON_DB_UPDATE:	
						mpd_database_update_dir(mpd, "/");
						break;
					case BUTTON_WEATHER_FORECAST_PREV:	
						decr_forecast_idx();
						break;
					case BUTTON_WEATHER_FORECAST_NEXT:	
						incr_forecast_idx();
						break;
					}
				}
		}
	}	
	mpd_free(mpd);
	close_framebuffer();
}


