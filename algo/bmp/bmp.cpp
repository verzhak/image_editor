
#include "algo/bmp/bmp.h"

shared_ptr<c_bitmap> algo::load_bmp(const string & fname)
{
	uint8_t * p_buf;
	unsigned v, u, t, buf_size, height, width, file_line_width, bits_per_sample;
	shared_ptr<FILE> fl(fopen(fname.c_str(), "r"), fclose);
	FILE * p_fl = fl.get();
	shared_ptr<c_bitmap> bmp;
	shared_ptr<uint8_t> buf;
	BITMAPHEADER header;

	fread(& header, sizeof(BITMAPHEADER), 1, p_fl);

	if(header.bmfHeader.bfType != 0x4D42)
		throw string("Файл не является файлом формата BMP");

	bits_per_sample = header.bmiHeader.biBitCount;

	if((bits_per_sample != 8 && bits_per_sample != 24) || header.bmiHeader.biPlanes != 1 || header.bmiHeader.biCompression)
		throw string("Файл использует особенности формата BMP, не поддерживаемые программой");

	height = header.bmiHeader.biHeight;
	width = header.bmiHeader.biWidth;
	file_line_width = width * (bits_per_sample / 8);
	file_line_width += (file_line_width & 3) ? 4 - (file_line_width & 3) : 0;

	buf_size = header.bmfHeader.bfSize - header.bmfHeader.bfOffBits;
	buf.reset(new uint8_t[buf_size]);
	p_buf = buf.get() + file_line_width * (height - 1);

	fseek(p_fl, header.bmfHeader.bfOffBits, SEEK_SET);
	fread(buf.get(), buf_size, 1, p_fl);

	bmp.reset(new c_bitmap(height, width));

	switch(bits_per_sample)
	{
		case 8:
		{
			uint32_t color, buf_palette[256];
		
			fseek(p_fl, sizeof(BITMAPHEADER), SEEK_SET);
			fread(buf_palette, 4, 256, p_fl);

			for(v = 0; v < height; v++, p_buf -= file_line_width)
				for(u = 0; u < width; u++)
				{
					color = buf_palette[p_buf[u]];

					bmp->pixel(v, u).set_u8(B, color & 0xFF);
					bmp->pixel(v, u).set_u8(G, (color & 0xFF00) >> 8);
					bmp->pixel(v, u).set_u8(R, (color & 0xFF0000) >> 16);
				}

			break;
		}
		case 24:
		{
			for(v = 0; v < height; v++, p_buf -= file_line_width)
				for(u = 0, t = 0; u < width; u++, t += 3)
				{
					bmp->pixel(v, u).set_u8(B, p_buf[t]);
					bmp->pixel(v, u).set_u8(G, p_buf[t + 1]);
					bmp->pixel(v, u).set_u8(R, p_buf[t + 2]);
				}
			
			break;
		}
	}

	return bmp;
}

void algo::save_bmp(c_bitmap & bmp, t_channel ch, const string & fname)
{
	unsigned v, u, t, buf_size, ul_y = bmp.aoi().ul_y(), ul_x = bmp.aoi().ul_x(), lr_y = bmp.aoi().lr_y(), lr_x = bmp.aoi().lr_x();
	unsigned height = bmp.aoi().height(), width = bmp.aoi().width();
	unsigned file_line_width = width * ((ch == ALL_CHANNEL) ? 3 : 1);
	shared_ptr<FILE> fl(fopen(fname.c_str(), "w"), fclose); // TODO
	shared_ptr<uint8_t> buf;
	uint8_t * p_buf;
	FILE * p_fl = fl.get();
	BITMAPHEADER header;

	if(file_line_width & 3)
		file_line_width += 4 - (file_line_width & 3);

	buf_size = file_line_width * height;
	buf.reset(new uint8_t[buf_size]);
	p_buf = buf.get() + file_line_width * (height - 1);

	header.bmfHeader.bfType = 0x4D42;
	header.bmfHeader.bfOffBits = sizeof(BITMAPHEADER) + 256 * 4 * ((ch == ALL_CHANNEL) ? 0 : 1);
	header.bmfHeader.bfSize = header.bmfHeader.bfOffBits + buf_size;
	header.bmfHeader.bfReserved1 = 0;
	header.bmfHeader.bfReserved2 = 0;

	header.bmiHeader.biSize = 40;
	header.bmiHeader.biWidth = width;
	header.bmiHeader.biHeight = height;
	header.bmiHeader.biPlanes = 1;
	header.bmiHeader.biBitCount = (ch == ALL_CHANNEL) ? 24 : 8;
	header.bmiHeader.biCompression = 0;
	header.bmiHeader.biSizeImage = 0;
	header.bmiHeader.biXPelsPerMeter = 160;
	header.bmiHeader.biYPelsPerMeter = 160;
	header.bmiHeader.biClrUsed = 0;
	header.bmiHeader.biClrImportant = 0;

	fwrite(& header, sizeof(BITMAPHEADER), 1, p_fl);

	if(ch == ALL_CHANNEL)
	{
		for(v = ul_y; v < lr_y; v++, p_buf -= file_line_width)
			for(u = ul_x, t = 0; u < lr_x; u++, t += 3)
			{
				p_buf[t] = bmp.pixel(v, u).get_u8(B);
				p_buf[t + 1] = bmp.pixel(v, u).get_u8(G);
				p_buf[t + 2] = bmp.pixel(v, u).get_u8(R);
			}
	}
	else
	{
		uint32_t buf_palette[256];
	
		for(u = 0; u < 256; u++)
			buf_palette[u] = (((uint32_t) u) << 16) + (((uint32_t) u) << 8) + ((uint32_t) u);

		fwrite(buf_palette, 4, 256, p_fl);

		for(v = ul_y; v < lr_y; v++, p_buf -= file_line_width)
			for(u = ul_x, t = 0; u < lr_x; u++, t++)
				p_buf[t] = bmp.pixel(v, u).get_u8(ch);
	}

	fwrite(buf.get(), 1, buf_size, p_fl);
}

