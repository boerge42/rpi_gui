/*
  Framebuffer-Lib fuer Raspberry Pi 
  =================================
          Uwe Berger; 2014

  Display-Hardware:
  --> https://github.com/watterott/RPi-Display
 
  Aufbauend/Inspiriert auf/durch:
  --> http://ozzmaker.com/2013/09/07/programming-a-touchscreen-on-the-raspberry-pi/
  --> https://github.com/kergoth/tslib
 
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

#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#include <linux/fb.h>
#include <linux/vt.h>
#include <linux/kd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <tslib.h>

#include "fonts.h"

#define FB_DEVICE 	"/dev/fb1" 
#define CON_DEVICE 	"/dev/tty1"

// default framebuffer palette
typedef enum {
  BLACK        =  0, /*   0,   0,   0 */
  BLUE         =  1, /*   0,   0, 172 */
  GREEN        =  2, /*   0, 172,   0 */
  CYAN         =  3, /*   0, 172, 172 */
  RED          =  4, /* 172,   0,   0 */
  PURPLE       =  5, /* 172,   0, 172 */
  ORANGE       =  6, /* 172,  84,   0 */
  LIGHT_GREY   =  7, /* 172, 172, 172 */
  GREY         =  8, /*  84,  84,  84 */
  LIGHT_BLUE   =  9, /*  84,  84, 255 */
  LIGHT_GREEN  = 10, /*  84, 255,  84 */
  LIGHT_CYAN   = 11, /*  84, 255, 255 */
  LIGHT_RED    = 12, /* 255,  84,  84 */
  LIGHT_PURPLE = 13, /* 255,  84, 255 */
  YELLOW       = 14, /* 255, 255,  84 */
  WHITE        = 15  /* 255, 255, 255 */
} COLOR_INDEX_T;

static const unsigned short def_r[] =
    { 0,   0,   0,   0, 172, 172, 172, 168,
     84,  84,  84,  84, 255, 255, 255, 255};

static const unsigned short def_g[] =
    { 0,   0, 168, 168,   0,   0,  84, 168,
     84,  84, 255, 255,  84,  84, 255, 255};

static const unsigned short def_b[] =
    { 0, 172,   0, 168,   0, 172,   0, 168,
     84, 255,  84, 255,  84, 255,  84, 255};

// Definition Button-Struktur
#define BUTTON_ACTIVE	0x0001	// 0b00000001 --> sichtbar, gedrueckt
#define BUTTON_VISIBLE	0x0002	// 0b00000010 --> sichtbar, nicht gedrueckt, nicht on
#define BUTTON_ON		0x0004	// 0b00000100 --> sichtbar, nicht gedrueckt
typedef struct  {
	int x, y, w, h;
	char *text;
	unsigned char na_fr_color, na_bg_color, na_txt_color; 
	unsigned char txt_font;
	unsigned char a_fr_color, a_bg_color, a_txt_color;
	unsigned char on_fr_color, on_bg_color, on_txt_color;
	unsigned char flags;
} button_t;

// *********************************
// *********************************
// *********************************
void draw_pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);
void draw_fillrect(int x1, int y1,int x2, int y2, unsigned char c);
void draw_line (int x1, int y1, int x2, int y2, unsigned char c);
void draw_rect (int x1, int y1, int x2, int y2, unsigned char c);
void clear_screen(void);
void draw_button(button_t *b);
int  button_handle (button_t *b, struct ts_sample *samp);
int  open_framebuffer(int *xres, int *yres);
void close_framebuffer(void);
void put_char(int x, int y, int ch, unsigned char c, unsigned char f);
void put_string(int x, int y, char *s, unsigned char c, unsigned char f);
void put_string_center(int x, int y, char *s, unsigned char c, unsigned char f);

#endif
