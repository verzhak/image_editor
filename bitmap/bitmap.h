
#ifndef BITMAP_H
#define BITMAP_H

#include "all.h"
#include "bitmap/color.h"

class c_aoi
{
	unsigned y, x, h, w;
	const unsigned image_height, image_width;
		
	public:

		c_aoi(unsigned t_image_height, unsigned t_image_width);

		inline unsigned ul_y() { return y; };
		inline unsigned ul_x() { return x; };
		inline unsigned lr_y() { return (y + h); };
		inline unsigned lr_x() { return (x + w); };
		inline unsigned height() { return h; };
		inline unsigned width() { return w; };

		void reset();
		void reset(unsigned t_y, unsigned t_x, unsigned t_height, unsigned t_width);
};

class c_bitmap
{
	void * mem;
	c_color ** data;
	c_aoi * raoi;

	void create();

	public:

		const unsigned height, width;

		c_bitmap(unsigned t_height, unsigned t_width);
		c_bitmap(c_bitmap & from);

		~c_bitmap();

		inline c_aoi & aoi() { return * raoi; };
		c_color & pixel(unsigned y, unsigned x);
};

#endif

