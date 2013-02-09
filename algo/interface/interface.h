
#ifndef INTERFACE_H
#define INTERFACE_H

#include "all.h"
#include "bitmap/bitmap.h"

extern "C"
{
	c_bitmap * create(unsigned height, unsigned width);
	double get(c_bitmap * bmp, unsigned y, unsigned x, unsigned ch_num);
	void set(c_bitmap * bmp, unsigned y, unsigned x, unsigned ch_num, double value);
	
	unsigned height(c_bitmap * bmp);
	unsigned width(c_bitmap * bmp);

	unsigned ul_y(c_bitmap * bmp);
	unsigned ul_x(c_bitmap * bmp);
	unsigned lr_y(c_bitmap * bmp);
	unsigned lr_x(c_bitmap * bmp);
	unsigned aoi_height(c_bitmap * bmp);
	unsigned aoi_width(c_bitmap * bmp);
}

#endif

