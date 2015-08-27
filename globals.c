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

#include "globals.h"

//**********************************************************************
int file_exist(char *fname) 
{
	struct stat buf;

	return (stat(fname, &buf) == 0); 
}

//**********************************************************************
void filename_with_underlines(char *fname_dest, char *fname_src)
{
	int i;
	
	strncpy(fname_dest, fname_src, sizeof(fname_src));
	for (i=0; i<strlen(fname_dest); i++) {
		if (fname_dest[i] == ' ') fname_dest[i] = '_';
	}
	
}

//**********************************************************************
void path_normalize (char *path_dest, char *path_src)
{
	int path_len;
	
	strncpy(path_dest, path_src, sizeof(path_src));
	path_len = strlen(path_dest);
	if (path_dest[path_len - 1] != '/') {
		path_dest[path_len] = '/';
		path_dest[path_len + 1] = '\0';
	}
}

// ********************************************
void jpeg_output(const char * fname, int x0, int y0, int dx, int dy)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE *infile;
	JSAMPARRAY buffer;
	int i,j,x=x0,y=y0;
	
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	if((infile = fopen(fname,"rb")) == NULL) {
		printf("Error: can't open file: %s\n", fname);
		return;
	}
	jpeg_stdio_src(&cinfo, infile);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);
	buffer=(*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, cinfo.output_width*cinfo.output_components, 1);	
	i=cinfo.output_width*cinfo.output_components*sizeof(char);
	while(cinfo.output_scanline < cinfo.output_height) {
		jpeg_read_scanlines(&cinfo, buffer, 1);
		j=0;
		while(j<=i)	{
			if ((x<(x0+dx))&&((y<(y0+dy))))
				draw_pixel(x, y, buffer[0][j],buffer[0][j+1],buffer[0][j+2]);
			j+=3;
			x++;
		}
		y++;
		x=x0;
	}	
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	fclose(infile);
}
