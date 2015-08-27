/*
  Font-Definitionen
  =================
   Uwe Berger; 2014

  ... fuer Framebuffer-Lib...
 
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

#ifndef _FONTS_H_
#define _FONTS_H_


#include "font_8x8.h"
#include "font_8x16.h"


typedef struct  {
   int width, height;
   unsigned char *data;
} font_t;


#define FONT_8X8	0
#define FONT_8X16	1

static const font_t font[2] = {
	{8,  8, font_8x8},
	{8, 16, font_8x16}
};



#endif /* _FONTS_H_ */
