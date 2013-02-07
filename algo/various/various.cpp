
#include "algo/algo.h"

void algo::get_min_max(c_bitmap & bmp, c_color & min, c_color & max, bool in_aoi)
{
	unsigned v, u, t, ul_y, ul_x, lr_y, lr_x;
	double value;

	if(in_aoi)
	{
		ul_y = bmp.aoi().ul_y();
		ul_x = bmp.aoi().ul_x();
		lr_y = bmp.aoi().lr_y();
		lr_x = bmp.aoi().lr_x();
	}
	else
	{
		ul_y = 0;
		ul_x = 0;
		lr_y = bmp.height;
		lr_x = bmp.width;
	}

	min.set_all(c_color::max_ch);
	max.set_all(c_color::min_ch);

	for(v = ul_y; v < lr_y; v++)
		for(u = ul_x; u < lr_x; u++)
			for(t = 0; t < CH_NUM; t++)
			{
				value = bmp.pixel(v, u).get(t);

				if(value > max.get(t))
					max.set(t, value);

				if(value < min.get(t))
					min.set(t, value);
			}
}

void algo::contrast(c_bitmap & bmp)
{
	c_color min, max;
	char sgn[CH_NUM];
	unsigned v, u, t, ul_y = bmp.aoi().ul_y(), ul_x = bmp.aoi().ul_x(), lr_y = bmp.aoi().lr_y(), lr_x = bmp.aoi().lr_x();
	double value, d1[CH_NUM], d2[CH_NUM];

	get_min_max(bmp, min, max, true);

	for(u = 0; u < CH_NUM; u++)
	{
		sgn[u] = 0;

		if(min.get(u) < 0)
			sgn[u] = -1;

		if(max.get(u) > 0)
			sgn[u] ++;

		if(sgn[u] > 0)
			d1[u] = c_color::max_ch / (max.get(u) - min.get(u));
		else if(sgn[u] < 0)
			d1[u] = fabs(c_color::min_ch) / (max.get(u) - min.get(u));
		else
		{
			d1[u] = c_color::max_ch / max.get(u);
			d2[u] = c_color::min_ch / min.get(u);
		}
	}

	for(v = ul_y; v < lr_y; v++)
		for(u = ul_x; u < lr_x; u++)
			for(t = 0; t < CH_NUM; t++)
			{
				if(sgn[t] > 0)
					bmp.pixel(v, u).set(t, (bmp.pixel(v, u).get(t) - min.get(t)) * d1[t]);
				else if (sgn[t] < 0)
					bmp.pixel(v, u).set(t, c_color::min_ch + (bmp.pixel(v, u).get(t) - min.get(t)) * d1[t]);
				else
				{
					value = bmp.pixel(v, u).get(t);

					if(value < 0)
						bmp.pixel(v, u).set(t, value * d2[t]);
					else
						bmp.pixel(v, u).set(t, value * d1[t]);
				}
			}
}

shared_ptr<c_bitmap> algo::rotate(c_bitmap & bmp, double alpha, double y0, double x0)
{
	int ty, tx;
	unsigned v, u, ul_y = bmp.aoi().ul_y(), ul_x = bmp.aoi().ul_x(), lr_y = bmp.aoi().lr_y(), lr_x = bmp.aoi().lr_x();
	unsigned height = bmp.aoi().height(), width = bmp.aoi().width();
	double dy, dx, y, x, ny, nx, min_y, min_x, cos_a = cos(alpha), sin_a = sin(alpha);
	double height_res = width * abs(sin_a) + height * abs(cos_a) + 0.5, width_res = width * abs(cos_a) + height * abs(sin_a) + 0.5;
	shared_ptr<c_bitmap> res(new c_bitmap(height_res + 1, width_res + 1));

	dy = y0 * cos_a - x0 * sin_a;
	dx = x0 * cos_a + y0 * sin_a;

	y = ul_y;
	x = ul_x;
	ny = - x * sin_a + y * cos_a - dy;
	nx = x * cos_a + y * sin_a - dx;
	min_y = ny;
	min_x = nx;

	y = lr_y;
	x = ul_x;
	ny = - x * sin_a + y * cos_a - dy;
	nx = x * cos_a + y * sin_a - dx;

	if(min_y > ny)
		min_y = ny;

	if(min_x > nx)
		min_x = nx;

	y = lr_y;
	x = lr_x;
	ny = - x * sin_a + y * cos_a - dy;
	nx = x * cos_a + y * sin_a - dx;

	if(min_y > ny)
		min_y = ny;

	if(min_x > nx)
		min_x = nx;

	y = ul_y;
	x = lr_x;
	ny = - x * sin_a + y * cos_a - dy;
	nx = x * cos_a + y * sin_a - dx;

	if(min_y > ny)
		min_y = ny;

	if(min_x > nx)
		min_x = nx;

	dy = min_x * sin_a + min_y * cos_a + y0;
	dx = min_x * cos_a - min_y * sin_a + x0;

	for(v = 0; v < height_res; v++)
		for(u = 0; u < width_res; u++)
		{
			ty = ceil(u * sin_a + v * cos_a + dy + 0.5);
			tx = ceil(u * cos_a - v * sin_a + dx + 0.5);

			if(
				(ty >= ul_y && ty < lr_y)
				&&
				(tx >= ul_x && tx < lr_x)
			  )
				res->pixel(v, u) = bmp.pixel(ty, tx);
		}

	return res;
}

void algo::diff(c_bitmap & bmp_1, c_bitmap & bmp_2)
{
	unsigned ul_y[2] = { bmp_1.aoi().ul_y(), bmp_2.aoi().ul_y() }, ul_x[2] = { bmp_1.aoi().ul_x(), bmp_2.aoi().ul_x() };
	unsigned r_height = bmp_1.aoi().height() > bmp_2.aoi().height() ? bmp_2.aoi().height() : bmp_1.aoi().height();
	unsigned r_width = bmp_1.aoi().width() > bmp_2.aoi().width() ? bmp_2.aoi().width() : bmp_1.aoi().width();
	unsigned v, u, t, q, bits_u8 = 0, bits_u16 = 0, bits_u32 = 0, r_v[2], r_u[2];
	uint8_t u8;
	uint16_t u16;
	uint32_t u32;

	for(t = 0; t < CH_NUM; t++)
		for(v = 0, r_v[0] = ul_y[0], r_v[1] = ul_y[1]; v < r_height; v++, r_v[0]++, r_v[1]++)
			for(u = 0, r_u[0] = ul_x[0], r_u[1] = ul_x[1]; u < r_width; u++, r_u[0]++, r_u[1]++)
			{
				u8 = bmp_1.pixel(r_v[0], r_u[0]).get_u8(t) ^ bmp_2.pixel(r_v[1], r_u[1]).get_u8(t);
				u16 = bmp_1.pixel(r_v[0], r_u[0]).get_u16(t) ^ bmp_2.pixel(r_v[1], r_u[1]).get_u16(t);
				u32 = bmp_1.pixel(r_v[0], r_u[0]).get_u32(t) ^ bmp_2.pixel(r_v[1], r_u[1]).get_u32(t);

				for(q = 0; q < 8; q ++, u8 >>= 1, u16 >>= 1, u32 >>= 1)
				{
					bits_u8 += u8 & 1;
					bits_u16 += u16 & 1;
					bits_u32 += u32 & 1;
				}

				for(; q < 16; q ++, u16 >>= 1, u32 >>= 1)
				{
					bits_u16 += u16 & 1;
					bits_u32 += u32 & 1;
				}

				for(; q < 32; q ++, u32 >>= 1)
					bits_u32 += u32 & 1;
			}

	printf("\nРазмеры областей интересов изображений: [%u строк, %u столбцов] и [%u строк, %u столбцов]\n\n\
В меньшей области интереса у изображений различны:\n\n\
\tРазмер канала: 8 бит, количество каналов: 3 -> %u бит = %lf байт = %lf килобайт\n\
\tРазмер канала: 8 бит, количество каналов: 1 -> %u бит = %lf байт = %lf килобайт\n\
\tРазмер канала: 16 бит, количество каналов: 3 -> %u бит = %lf байт = %lf килобайт\n\
\tРазмер канала: 16 бит, количество каналов: 1 -> %u бит = %lf байт = %lf килобайт\n\
\tРазмер канала: 32 бит, количество каналов: 3 -> %u бит = %lf байт = %lf килобайт\n\
\tРазмер канала: 32 бит, количество каналов: 1 -> %u бит = %lf байт = %lf килобайт\n\n",
		bmp_1.aoi().height(), bmp_1.aoi().width(), bmp_2.aoi().height(), bmp_2.aoi().width(),
		bits_u8, bits_u8 / 8.0, bits_u8 / 8192.0, bits_u8 / 3, bits_u8 / 24.0, bits_u8 / 24576.0,
		bits_u16, bits_u16 / 8.0, bits_u16 / 8192.0, bits_u16 / 3, bits_u16 / 24.0, bits_u16 / 24576.0,
		bits_u32, bits_u32 / 8.0, bits_u32 / 8192.0, bits_u32 / 3, bits_u32 / 24.0, bits_u32 / 24576.0);
}

shared_ptr<c_bitmap> algo::resize(c_bitmap & bmp, double k)
{
	unsigned tv, tu, v, u, ul_y = bmp.aoi().ul_y(), ul_x = bmp.aoi().ul_x(), lr_y = bmp.aoi().lr_y(), lr_x = bmp.aoi().lr_x();
	unsigned height_res = ceil(k * bmp.aoi().height() + 0.5), width_res = ceil(k * bmp.aoi().width() + 0.5);
	shared_ptr<c_bitmap> res(new c_bitmap(height_res, width_res));

	for(v = 0; v < height_res; v++)
		for(u = 0; u < width_res; u++)
		{
			tv = ceil(ul_y + v / k + 0.5);
			tu = ceil(ul_x + u / k + 0.5);

			if(
				tv < lr_y
				&&
				tu < lr_x
			  )
				res->pixel(v, u) = bmp.pixel(tv, tu);
		}

	return res;
}

