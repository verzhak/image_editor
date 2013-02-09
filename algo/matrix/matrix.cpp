
#include "algo/algo.h"

shared_ptr<c_bitmap> algo::load_matrix_16(const string & fname, unsigned height, unsigned width)
{
	int v_inv;
	unsigned u, v, u_2, width_2 = width >> 1;
	shared_ptr<c_bitmap> bmp(new c_bitmap(height, width));
	shared_ptr<uint8_t> buf(new uint8_t[height * width_2]);
	uint8_t * p_buf = buf.get();
	c_file fl(fname, MODE_READ);

	fl.read(p_buf, height * width_2);

	for(v = 0, v_inv = height - 1; v < height; v++, v_inv--)
		for(u = 0, u_2 = 0; u < width_2; u++, u_2++)
		{
			bmp->pixel(v, u_2).set_all_u8(p_buf[v_inv * width_2 + u] & 0xF0);

			u_2 ++;

			bmp->pixel(v, u_2).set_all_u8((p_buf[v_inv * width_2 + u] & 0x0F) << 4);
		}

	return bmp;
}

shared_ptr<c_bitmap> algo::load_matrix_256(const string & fname, unsigned height, unsigned width)
{
	int v_inv;
	unsigned u, v;
	shared_ptr<c_bitmap> bmp(new c_bitmap(height, width));
	shared_ptr<uint8_t> buf(new uint8_t[height * width]);
	uint8_t * p_buf = buf.get();
	c_file fl(fname, MODE_READ);

	fl.read(p_buf, height * width);

	for(v = 0, v_inv = height - 1; v < height; v++, v_inv--)
		for(u = 0; u < width; u++)
			bmp->pixel(v, u).set_all_u8(p_buf[v_inv * width + u]);

	return bmp;
}

