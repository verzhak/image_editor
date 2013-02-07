
#include "algo/algo.h"

shared_ptr<c_bitmap> algo::radiometric_correction(c_bitmap & bmp, unsigned N)
{
	int v, u, from, to;
	unsigned tv, q, med, sc, height = bmp.aoi().height(), width = bmp.aoi().width(), N_2 = N >> 1;
	double sum, dd, r, m, d[CH_NUM][N];
	shared_ptr<c_bitmap> res(new c_bitmap(height, width));

	// ############################################################################ 
	// Строки
	
	for(v = 0; v < height; v++)
	{
		from = v - N_2;
		to = v + N_2;

		if(from < 0)
			from = 0;

		if(to >= height)
			to = height - 1;

		for(tv = from, sc = 0; tv <= to; tv++, sc++)
		{
			if(tv == v)
				med = sc;

			for(q = 0; q < CH_NUM; q++)
			{
				m = 0;

				for(u = 0; u < width; u++)
					m += bmp.pixel(tv, u)[q];
				
				m /= (1.0 * width);
				
				d[q][sc] = 0;

				for(u = 0; u < width; u++)
				{
					r = bmp.pixel(tv, u)[q] - m;
					d[q][sc] += r * r;
				}

				d[q][sc] = sqrt(d[q][sc] / (1.0 * width));
			}
		}

		for(q = 0; q < CH_NUM; q++)
		{
			m = 0;

			for(tv = 0; tv < sc; tv++)
				m += d[q][tv];

			m /= (1.0 + sc);

			for(tv = 0, dd = 0; tv < sc; tv++)
			{
				r = d[q][tv] - m;
				dd += r * r;
			}

			dd = 3 * sqrt(dd / (1.0 * sc));

			if(d[q][med] > dd + m || d[q][med] < - dd + m)
				for(u = 0; u < width; u++)
				{
					for(tv = from, sum = 0; tv <= to; tv ++)
						if(v != tv)
							sum += bmp.pixel(tv, u)[q];

					res->pixel(v, u)[q] = sum / (1.0 * (to - from - 1));
				}
			else
				for(u = 0; u < width; u++)
					res->pixel(v, u)[q] = bmp.pixel(v, u)[q];
		}
	}

	// ############################################################################ 
	// Столбцы

	// TODO

	// ############################################################################ 

	return res;
}
	
