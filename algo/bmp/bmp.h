
#ifndef BMP_H
#define BMP_H

#include "algo/algo.h"

#define WORD uint16_t
#define DWORD uint32_t

struct __attribute__((packed)) BITMAPFILEHEADER
{
	WORD bfType;
	DWORD bfSize;
	WORD bfReserved1;
	WORD bfReserved2;
	DWORD bfOffBits;
};

struct __attribute__((packed)) BITMAPINFOHEADER
{
	DWORD biSize;
	DWORD biWidth;
	DWORD biHeight;
	WORD biPlanes;
	WORD biBitCount;
	DWORD biCompression;
	DWORD biSizeImage;
	DWORD biXPelsPerMeter;
	DWORD biYPelsPerMeter;
	DWORD biClrUsed;
	DWORD biClrImportant;
};

struct BITMAPHEADER
{
	BITMAPFILEHEADER bmfHeader;
	BITMAPINFOHEADER bmiHeader;
};

#endif

