
#include "bitmap/bitmap.h"

c_aoi::c_aoi(unsigned t_image_height, unsigned t_image_width) :
	image_height(t_image_height), image_width(t_image_width)
{
	reset();
}

void c_aoi::reset()
{
	reset(0, 0, image_height, image_width);
}

void c_aoi::reset(unsigned t_y, unsigned t_x, unsigned t_height, unsigned t_width)
{
	if(t_y + t_height > image_height || t_x + t_width > image_width)
		throw string("Слишком большая область интереса");

	y = t_y;
	x = t_x;
	h = t_height;
	w = t_width;
}

// ############################################################################ 

c_bitmap::c_bitmap(unsigned t_height, unsigned t_width) :
	mem(NULL), data(NULL), raoi(NULL), height(t_height), width(t_width)
{
	create();
}

c_bitmap::c_bitmap(c_bitmap & from) :
	mem(NULL), data(NULL), raoi(NULL), height(from.aoi().height()), width(from.aoi().width())
{
	unsigned v, u, from_v, from_u, from_ul_y = from.aoi().ul_y(), from_ul_x = from.aoi().ul_x();

	create();

	for(v = 0, from_v = from_ul_y; v < height; v++, from_v++)
		for(u = 0, from_u = from_ul_x; u < width; u++, from_u++)
			pixel(v, u) = from.pixel(from_v, from_u);
}

c_bitmap::~c_bitmap()
{
	if(mem != NULL)
	{
		unsigned v, u;

		raoi->~c_aoi();

		for(v = 0; v < height; v++)
			for(u = 0; u < width; u++)
				data[v][u].~c_color();

		munmap(mem, sizeof(c_aoi) + height * (sizeof(c_color *) + width * sizeof(c_color)));
	}
}

void c_bitmap::create()
{
	unsigned v, u;
	c_color * pnt;

	mem = mmap(NULL, sizeof(c_aoi) + height * (sizeof(c_color *) + width * sizeof(c_color)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	if(mem == NULL)
		throw string("Ошибка при выделении памяти под внутреннее представление пикселей");

	raoi = (c_aoi *) mem;
	data = (c_color **) (raoi + 1);

	raoi = new (raoi) c_aoi(height, width);

	for(v = 0, pnt = (c_color *) (data + height); v < height; v++)
	{
		data[v] = pnt;

		for(u = 0; u < width; u++, pnt++)
			new (pnt) c_color;
	}
}

c_color & c_bitmap::pixel(unsigned y, unsigned x)
{
	if(y < height && x < width)
		return data[y][x];

	throw string("Некорректные индексы пикселя");
}

