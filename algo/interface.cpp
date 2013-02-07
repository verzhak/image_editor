
#include "algo/interface.h"

c_bitmap * create(unsigned height, unsigned width)
{
	return new c_bitmap(height, width);
}

double get(c_bitmap * bmp, unsigned y, unsigned x, unsigned ch_num)
{
	return bmp->pixel(y, x).get(ch_num);
}

void set(c_bitmap * bmp, unsigned y, unsigned x, unsigned ch_num, double value)
{
	bmp->pixel(y, x).set(ch_num, value);
}

unsigned height(c_bitmap * bmp)
{
	return bmp->height;
}

unsigned width(c_bitmap * bmp)
{
	return bmp->width;
}

unsigned ul_y(c_bitmap * bmp)
{
	return bmp->aoi().ul_y();
}

unsigned ul_x(c_bitmap * bmp)
{
	return bmp->aoi().ul_x();
}

unsigned lr_y(c_bitmap * bmp)
{
	return bmp->aoi().lr_y();
}

unsigned lr_x(c_bitmap * bmp)
{
	return bmp->aoi().lr_x();
}

unsigned aoi_height(c_bitmap * bmp)
{
	return bmp->aoi().height();
}

unsigned aoi_width(c_bitmap * bmp)
{
	return bmp->aoi().width();
}

