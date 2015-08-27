/*
  Routinen fuer Diashow 
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

#ifndef _DIASHOW_SCREEN_H_
#define _DIASHOW_SCREEN_H_

#include <stdio.h>
#include <dirent.h>
#include <fnmatch.h>

#include "framebuffer.h"
#include "globals.h"

extern char cfg_diashow_path[255];
extern char cfg_diashow_file_pattern[255];

// ***************************************************************
void next_photo(void);

#endif