/*
  ein paar allgemeine Routinen... 
  ===============================
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

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <jpeglib.h>

#include "framebuffer.h"

// ***************************************************************
int file_exist(char *fname);
void filename_with_underlines(char *fname_dest, char *fname_src);
void path_normalize (char *path_dest, char *path_src);
void jpeg_output(const char * fname, int x0, int y0, int dx, int dy);

#endif
