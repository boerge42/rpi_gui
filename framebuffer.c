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

#include "framebuffer.h"

static struct fb_fix_screeninfo fix;
static struct fb_var_screeninfo orig_var;
static struct fb_var_screeninfo var;
static char *fbp = 0;
static int fb;
static int tty;
static long int screensize = 0;

static int max_x = 0;
static int max_y = 0;


// ************************************
void clear_screen(void) 
{
	// Bildschirm loeschen
	memset(fbp,0,fix.smem_len);	
}

// ************************************
void draw_pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b)
{
	unsigned int pix_offset;
	unsigned short c;

	if ((x <= max_x) && (y <= max_y) && (x>=0) && (y>=0)) {
		// calculate the pixel's byte offset inside the buffer
		pix_offset = x*2 + y * fix.line_length;
		//some magic to work out the color
		c = ((r / 8) << 11) + ((g / 4) << 5) + (b / 8);
		// write 'two bytes at once'
		*((unsigned short*)(fbp + pix_offset)) = c;
	}
}

// ************************************
void draw_fillrect(int x1, int y1, int x2, int y2,  unsigned char c)
{
	int x, y;
	
	for ( y = y1; y <= y2; y++)
		for ( x = x1; x <=  x2; x++)
			draw_pixel( x, y, def_r[c],def_g[c],def_b[c]);
}

// ************************************
void draw_line (int x1, int y1, int x2, int y2, unsigned char c)
{
	int tmp;
	int dx = x2 - x1;
	int dy = y2 - y1;

	if (abs (dx) < abs (dy)) {
		if (y1 > y2) {
			tmp = x1; x1 = x2; x2 = tmp;
			tmp = y1; y1 = y2; y2 = tmp;
			dx = -dx; dy = -dy;
		}
		x1 <<= 16;
		/* dy is apriori >0 */
		dx = (dx << 16) / dy;
		while (y1 <= y2) {
			draw_pixel(x1 >> 16, y1, def_r[c],def_g[c],def_b[c]);
			x1 += dx;
			y1++;
		}
	} else {
		if (x1 > x2) {
			tmp = x1; x1 = x2; x2 = tmp;
			tmp = y1; y1 = y2; y2 = tmp;
			dx = -dx; dy = -dy;
		}
		y1 <<= 16;
		dy = dx ? (dy << 16) / dx : 0;
		while (x1 <= x2) {
			draw_pixel(x1, y1 >> 16, def_r[c],def_g[c],def_b[c]);
			y1 += dy;
			x1++;
		}
	}
}

// ************************************
void draw_rect (int x1, int y1, int x2, int y2, unsigned char c)
{
	draw_line (x1, y1, x2, y1, c);
	draw_line (x2, y1, x2, y2, c);
	draw_line (x2, y2, x1, y2, c);
	draw_line (x1, y2, x1, y1, c);
}

// ************************************
int open_framebuffer(int *xres, int *yres)
{
	// Konsole in Grafik-Mode schalten
	tty = open(CON_DEVICE, O_RDWR);
	if (tty == -1) {
		perror("open console");
		return -1;		
	}
	if(ioctl(tty, KDSETMODE, KD_GRAPHICS) == -1)
		perror("set graphics mode on console");
	// Framebuffer-Device oeffnen etc.
	fb = open(FB_DEVICE, O_RDWR);
	if (fb == -1) {
		perror("open fbdevice");
		return -1;
	}
	if (ioctl(fb, FBIOGET_FSCREENINFO, &fix) < 0) {
		perror("ioctl FBIOGET_FSCREENINFO");
		close(fb);
		return -1;
	}
	if (ioctl(fb, FBIOGET_VSCREENINFO, &var) < 0) {
		perror("ioctl FBIOGET_VSCREENINFO");
		close(fb);
		return -1;
	}

#if DEBUG
	printf("Original %dx%d, %dbpp\n", var.xres, var.yres, 
         var.bits_per_pixel );
#endif

	memcpy(&orig_var, &var, sizeof(struct fb_var_screeninfo));

#if DEBUG
	printf("Framebuffer %s resolution;\n", fbdevice);
  	printf("%dx%d, %d bpp\n\n\n", var.xres, var.yres, var.bits_per_pixel );
#endif

	// Framebuffer in User-Memory mappen
	screensize = fix.smem_len;
	fbp = (char*)mmap(0, 
                    screensize, 
                    PROT_READ | PROT_WRITE, 
                    MAP_FILE | MAP_SHARED, 
                    fb, 0);
	if ((int)fbp == -1) {
		printf("Failed to mmap.\n");
		close(fb);
		return -1;
	}
	clear_screen();
	*xres = var.xres;
	*yres = var.yres;
	// interne max. XY-Maximas (Verwendung in put_char()) setzen...
	max_x = var.xres - 1;
	max_y = var.yres - 1;  
	return 0;
}

// ************************************
void close_framebuffer(void)
{
	clear_screen();
	// Framebuffer schliessen etc.
	munmap(fbp, screensize);
	if (ioctl(fb, FBIOPUT_VSCREENINFO, &orig_var)) {
		perror("reset screeninfo");
	}
	close(fb);
	// Konsole wieder herstellen
    if (tty >= 0) {
		if(ioctl(tty, KDSETMODE, KD_TEXT) == -1)
			perror("set text mode on console");
    }
}

// ************************************
void put_char(int x, int y, int ch, unsigned char c, unsigned char f)
{
	int i,j,bits;

	for (i = 0; i < font[f].height; i++) {
		bits = font[f].data [font[f].height * ch + i];
		for (j = 0; j < font[f].width; j++, bits <<= 1)
			if (bits & 0x80){
				draw_pixel(x+j, y+i, def_r[c], def_g[c], def_b[c]);
			}
	}
}

// ************************************
void put_string(int x, int y, char *s, unsigned char c, unsigned char f)
{
	int i;

	for (i = 0; *s; i++, x += font[f].width, s++)
		put_char (x, y, *s, c, f);
}

// ************************************
void put_string_center(int x, int y, char *s, unsigned char c, unsigned char f)
{
	size_t sl = strlen (s);

    put_string (x - (sl / 2) * font[f].width,
                y - font[f].height / 2, s, c, f);
}

// ************************************
void draw_button(button_t *b) 
{
	unsigned char bg_color, fr_color, txt_color;

	// soll Button ueberhaupt sichtbar sein?
	if (!(b->flags & BUTTON_VISIBLE)) return;
	// welche Farben?
	if (b->flags & BUTTON_ACTIVE) {
		bg_color  = b->a_bg_color;	
		fr_color  = b->a_fr_color;	
		txt_color = b->a_txt_color;	
	} else {
		if (b->flags & BUTTON_ON) {
			bg_color  = b->on_bg_color;	
			fr_color  = b->on_fr_color;	
			txt_color = b->on_txt_color;	
		} else {
			bg_color  = b->na_bg_color;	
			fr_color  = b->na_fr_color;	
			txt_color = b->na_txt_color;	
		}
	}	
	// Hindergrund
	draw_fillrect(b->x, b->y, (b->x+b->w), (b->y+b->h), bg_color);
	// Frame
	draw_rect(b->x, b->y, (b->x+b->w), (b->y+b->h), fr_color);
	// Text (zentiert)
	put_string_center((b->x+(b->w/2)), (b->y+(b->h/2)), b->text, txt_color, b->txt_font);
}

// ************************************
int  button_handle (button_t *b, struct ts_sample *samp)
{
	// Button ueberhaupt sichtbar?
	if (!(b->flags & BUTTON_VISIBLE)) return 0;
	
	// Touch innerhalb der Buttonflaeche?
	int inside = 	(samp->x >= b->x) && (samp->y >= b->y) &&
					(samp->x <  b->x + b->w) &&
					(samp->y <  b->y + b->h);
	if (samp->pressure > 0) {
		if (inside) {
			if (!(b->flags & BUTTON_ACTIVE)) {
				b->flags |= BUTTON_ACTIVE;
				draw_button(b);
			}
		} else if (b->flags & BUTTON_ACTIVE) {
			b->flags &= ~BUTTON_ACTIVE;
			draw_button(b);
		}
	} else if (b->flags & BUTTON_ACTIVE) {
		b->flags &= ~BUTTON_ACTIVE;
		draw_button(b);
		return 1;
	}
	return 0;	
}
