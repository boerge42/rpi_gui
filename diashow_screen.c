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

#include "diashow_screen.h"

// ********************************************
void next_photo(void) 
{
	static char last_photo[PATH_MAX] = "";
	char first_photo[PATH_MAX] = "";
	DIR *d;
	struct dirent *d_entry;
	char entry_path[PATH_MAX + 1];
	size_t path_len;
	
	strncpy (entry_path, cfg_diashow_path, sizeof (entry_path));
	path_normalize(entry_path, entry_path);
	path_len = strlen(entry_path);
	if ((d=opendir(entry_path)) == NULL) {
		printf("Error: can't open directory: %s\n", cfg_diashow_path);
		return;		
	}
	while ((d_entry = readdir(d)) != NULL) {
   		strncpy(entry_path + path_len, d_entry->d_name, sizeof (entry_path) - path_len);
		if (fnmatch(cfg_diashow_file_pattern, d_entry->d_name, 0) == 0) {
			// erstes Foto im Pfad merken
			if (!strlen(first_photo)) strncpy(first_photo, entry_path, strlen(entry_path)); 
			// letztes angezeigtes Foto?
			if (!strncmp(entry_path, last_photo, strlen(last_photo))) {
				// ...wenn ja und es noch ein weiteres Foto gibt, dann dieses,
				// ansonsten wieder das erste Foto
				d_entry = readdir(d);
				if ((d_entry != NULL)&&(fnmatch(cfg_diashow_file_pattern, d_entry->d_name, 0) == 0)) {
					strncpy(entry_path + path_len, d_entry->d_name, sizeof (entry_path) - path_len);
					strncpy(last_photo, entry_path, strlen(entry_path));
				} else {
					strncpy(last_photo, first_photo, strlen(first_photo));
				}
			} 
		}
 	}
	// falls Foto gefunden, anzeigen
	if (strlen(last_photo)) jpeg_output(last_photo, 0, 0, 320, 240);
	closedir(d);	
}
