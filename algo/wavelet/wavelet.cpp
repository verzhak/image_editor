
#include "algo/algo.h"

void algo::wavelet_down(c_bitmap & bmp, shared_ptr<c_bitmap> & A, shared_ptr<c_bitmap> & Dh, shared_ptr<c_bitmap> & Dv, shared_ptr<c_bitmap> & Dd, e_wavelet fun)
{
	unsigned v, u, tv, tu, ul_y = bmp.aoi().ul_y(), ul_x = bmp.aoi().ul_x(), lr_y = bmp.aoi().lr_y(), lr_x = bmp.aoi().lr_x();
	unsigned height = bmp.aoi().height(), width = bmp.aoi().width(), to_height, to_width;
	unsigned res_height = (height >> 1) + (height & 1), res_width = (width >> 1) + (width & 1);
	
	A.reset(new c_bitmap(res_height, res_width));
	Dh.reset(new c_bitmap(res_height, res_width));
	Dv.reset(new c_bitmap(res_height, res_width));
	Dd.reset(new c_bitmap(res_height, res_width));

	if(! (A && Dh && Dv && Dd))
		throw string("Ошибка при выделении памяти под результирующие изображения");

	if(fun == HAAR)
	{
		to_height = lr_y - (height & 1);
		to_width = lr_x - (width & 1);

		for(v = ul_y, tv = 0; v < to_height; v += 2, tv++)
		{
			for(u = ul_x, tu = 0; u < to_width; u += 2, tu++)
			{
				A->pixel(tv, tu) = bmp.pixel(v, u);
				A->pixel(tv, tu) += bmp.pixel(v, u + 1);
				A->pixel(tv, tu) += bmp.pixel(v + 1, u);
				A->pixel(tv, tu) += bmp.pixel(v + 1, u + 1);
				A->pixel(tv, tu) /= 4;

				Dh->pixel(tv, tu) = bmp.pixel(v, u);
				Dh->pixel(tv, tu) -= bmp.pixel(v, u + 1);
				Dh->pixel(tv, tu) += bmp.pixel(v + 1, u);
				Dh->pixel(tv, tu) -= bmp.pixel(v + 1, u + 1);
				Dh->pixel(tv, tu) /= 4;

				Dv->pixel(tv, tu) = bmp.pixel(v, u);
				Dv->pixel(tv, tu) += bmp.pixel(v, u + 1);
				Dv->pixel(tv, tu) -= bmp.pixel(v + 1, u);
				Dv->pixel(tv, tu) -= bmp.pixel(v + 1, u + 1);
				Dv->pixel(tv, tu) /= 4;

				Dd->pixel(tv, tu) = bmp.pixel(v, u);
				Dd->pixel(tv, tu) -= bmp.pixel(v, u + 1);
				Dd->pixel(tv, tu) -= bmp.pixel(v + 1, u);
				Dd->pixel(tv, tu) += bmp.pixel(v + 1, u + 1);
				Dd->pixel(tv, tu) /= 4;
			}

			if(to_width != lr_x)
			{
				tu = res_width - 1;
				u = lr_x - 1;

				A->pixel(tv, tu) = bmp.pixel(v, u);
				A->pixel(tv, tu) += bmp.pixel(v + 1, u);
				A->pixel(tv, tu) /= 4;

				Dh->pixel(tv, tu) = A->pixel(tv, tu);

				Dv->pixel(tv, tu) = bmp.pixel(v, u);
				Dv->pixel(tv, tu) -= bmp.pixel(v + 1, u);
				Dv->pixel(tv, tu) /= 4;

				Dd->pixel(tv, tu) = Dv->pixel(tv, tu);
			}
		}

		if(to_height != lr_y)
		{
			tv = res_height - 1;
			v = lr_y - 1;

			for(u = ul_x, tu = 0; u < to_width; u += 2, tu++)
			{
				A->pixel(tv, tu) = bmp.pixel(v, u);
				A->pixel(tv, tu) += bmp.pixel(v, u + 1);
				A->pixel(tv, tu) /= 4;

				Dh->pixel(tv, tu) = bmp.pixel(v, u);
				Dh->pixel(tv, tu) -= bmp.pixel(v, u + 1);
				Dh->pixel(tv, tu) /= 4;

				Dv->pixel(tv, tu) = A->pixel(tv, tu);

				Dd->pixel(tv, tu) = Dh->pixel(tv, tu);
			}

			if(to_width != lr_x)
			{
				tu = res_width - 1;
				u = lr_x - 1;

				A->pixel(tv, tu) = bmp.pixel(v, u);
				A->pixel(tv, tu) /= 4;

				Dh->pixel(tv, tu) = A->pixel(tv, tu);
				
				Dv->pixel(tv, tu) = A->pixel(tv, tu);
				
				Dd->pixel(tv, tu) = A->pixel(tv, tu);
			}
		}
	}
}

shared_ptr<c_bitmap> algo::wavelet_up(shared_ptr<c_bitmap> & A, shared_ptr<c_bitmap> & Dh, shared_ptr<c_bitmap> & Dv, shared_ptr<c_bitmap> & Dd, e_wavelet fun)
{
	unsigned v, u, tv, tu, res_height, res_width, height = 0, width = 0, ul_y[4], ul_x[4];
	c_color a, dv, dh, dd;
	shared_ptr<c_bitmap> bmp;

	#define WV_GET_DISP_SIZE(p_bmp, var, ind)\
	{\
		if((p_bmp))\
		{\
			if(! height)\
			{\
				height = (p_bmp)->aoi().height();\
				width = (p_bmp)->aoi().width();\
			}\
			else if(height != (p_bmp)->aoi().height() || width != (p_bmp)->aoi().width())\
				throw string("Изображения не совпадают размерами");\
	\
			ul_y[(ind)] = (p_bmp)->aoi().ul_y();\
			ul_x[(ind)] = (p_bmp)->aoi().ul_x();\
		}\
		else\
			var.set_all(0);\
	}

	WV_GET_DISP_SIZE(A, a, 0);
	WV_GET_DISP_SIZE(Dh, dh, 1);
	WV_GET_DISP_SIZE(Dv, dv, 2);
	WV_GET_DISP_SIZE(Dd, dd, 3);

	res_height = height << 1;
	res_width = width << 1;
	bmp.reset(new c_bitmap(res_height, res_width));

	if(fun == HAAR)
		for(tv = 0, v = 0; tv < height; tv++, v += 2)
			for(tu = 0, u = 0; tu < width; tu++, u += 2)
			{
				#define WV_GET_ELEM(p_bmp, var, ind)\
				{\
					if((p_bmp) != NULL)\
						(var) = (p_bmp)->pixel(ul_y[(ind)] + tv, ul_x[(ind)] + tu);\
				}

				WV_GET_ELEM(A, a, 0);
				WV_GET_ELEM(Dh, dh, 1);
				WV_GET_ELEM(Dv, dv, 2);
				WV_GET_ELEM(Dd, dd, 3);

				bmp->pixel(v, u) = a;
				bmp->pixel(v, u) += dh;
				bmp->pixel(v, u) += dv;
				bmp->pixel(v, u) += dd;

				bmp->pixel(v, u + 1) = a;
				bmp->pixel(v, u + 1) -= dh;
				bmp->pixel(v, u + 1) += dv;
				bmp->pixel(v, u + 1) -= dd;

				bmp->pixel(v + 1, u) = a;
				bmp->pixel(v + 1, u) += dh;
				bmp->pixel(v + 1, u) -= dv;
				bmp->pixel(v + 1, u) -= dd;

				bmp->pixel(v + 1, u + 1) = a;
				bmp->pixel(v + 1, u + 1) -= dh;
				bmp->pixel(v + 1, u + 1) -= dv;
				bmp->pixel(v + 1, u + 1) += dd;
			}

	return bmp;
}

