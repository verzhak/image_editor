
#include "algo/algo.h"

shared_ptr<c_bitmap> algo::fold(c_bitmap & bmp, double k, vector<vector<double> > & matrix, unsigned matrix_height, unsigned matrix_width)
{
	int v, u, from_v, to_v, from_u, to_u;
	unsigned from_mv, from_mu, q, mv, mu, tv, tu, height = bmp.aoi().height(), width = bmp.aoi().width();
	unsigned matrix_height_2 = matrix_height >> 1, matrix_width_2 = matrix_width >> 1;
	double sum[CH_NUM];
	shared_ptr<c_bitmap> new_bmp(new c_bitmap(height, width));

	for(v = 0; v < height; v++)
		for(u = 0; u < width; u++)
		{
			from_v = v - matrix_height_2;
			to_v = v + matrix_height_2;

			from_u = u - matrix_width_2;
			to_u = u + matrix_width_2;

			from_mv = from_mu = 0;

			if(from_v < 0)
			{
				from_mv = abs(from_v);
				from_v = 0;
			}

			if(to_v >= height)
				to_v = height - 1;

			if(from_u < 0)
			{
				from_mu = abs(from_u);
				from_u = 0;
			}

			if(to_u >= width)
				to_u = width - 1;

			for(q = 0; q < CH_NUM; q++)
				sum[q] = 0;

			for(tv = from_v, mv = from_mv; tv <= to_v; tv++, mv++)
				for(tu = from_u, mu = from_mu; tu <= to_u; tu++, mu++)
				{
					c_color & px = bmp.pixel(tv, tu);

					for(q = 0; q < CH_NUM; q++)
						sum[q] += px[q] * matrix[mv][mu];
				}

			for(q = 0; q < CH_NUM; q++)
				new_bmp->pixel(v, u).set(q, k * sum[q]);
		}

	return new_bmp;
}

shared_ptr<c_bitmap> algo::predef_fold(c_bitmap & bmp, e_matrix matrix)
{
	unsigned v, u, matrix_height, matrix_width;
	double k;
	vector<vector<double> > mtx;

#define PREDEF_FOLD_BEGIN(t_matrix_height, t_matrix_width, t_k)\
{\
k = (t_k);\
matrix_height = (t_matrix_height);\
matrix_width = (t_matrix_width);\
mtx.resize(t_matrix_height);\
for(auto & p : mtx)\
	p.resize(t_matrix_width);\
}

	switch(matrix)
	{
		case LOW_FREQ_3_3_1:
		{
			PREDEF_FOLD_BEGIN(3, 3, 1.0 / 9);

			for(v = 0; v < 3; v++)
				for(u = 0; u < 3; u++)
					mtx[v][u] = 1;

			break;
		}
		case LOW_FREQ_3_3_2_1:
		{
			PREDEF_FOLD_BEGIN(3, 3, 1.0 / 10);

			for(v = 0; v < 3; v++)
				for(u = 0; u < 3; u++)
					mtx[v][u] = 1;

			mtx[1][1] = 2;

			break;
		}
		case LOW_FREQ_5_5_1:
		{
			PREDEF_FOLD_BEGIN(5, 5, 1.0 / 25);

			for(v = 0; v < 5; v++)
				for(u = 0; u < 5; u++)
					mtx[v][u] = 1;

			break;
		}
		case LOW_FREQ_5_5_2_1:
		{
			PREDEF_FOLD_BEGIN(5, 5, 1.0 / 26);

			for(v = 0; v < 5; v++)
				for(u = 0; u < 5; u++)
					mtx[v][u] = 1;

			mtx[2][2] = 2;

			break;
		}
		case LOW_FREQ_7_7_1:
		{
			PREDEF_FOLD_BEGIN(7, 7, 1.0 / 49);

			for(v = 0; v < 7; v++)
				for(u = 0; u < 7; u++)
					mtx[v][u] = 1;

			break;
		}
		case LOW_FREQ_7_7_2_1:
		{
			PREDEF_FOLD_BEGIN(7, 7, 1.0 / 50);

			for(v = 0; v < 7; v++)
				for(u = 0; u < 7; u++)
					mtx[v][u] = 1;

			mtx[3][3] = 2;

			break;
		}

		case LAPLACIAN_3_3:
		{
			PREDEF_FOLD_BEGIN(3, 3, 1);

			mtx[0][1] = 1;
			mtx[1][0] = 1;
			mtx[1][1] = -4;
			mtx[1][2] = 1;
			mtx[2][1] = 1;

			break;
		}

		case NORTH_3_3:
		{
			PREDEF_FOLD_BEGIN(3, 3, 1);

			mtx[0][0] = 1;
			mtx[0][1] = 1;
			mtx[0][2] = 1;
			mtx[1][0] = 1;
			mtx[1][1] = -2;
			mtx[1][2] = 1;
			mtx[2][0] = -1;
			mtx[2][1] = -1;
			mtx[2][2] = -1;

			break;
		}
		case SOUTH_3_3:
		{
			PREDEF_FOLD_BEGIN(3, 3, 1);

			mtx[0][0] = -1;
			mtx[0][1] = -1;
			mtx[0][2] = -1;
			mtx[1][0] = 1;
			mtx[1][1] = -2;
			mtx[1][2] = 1;
			mtx[2][0] = 1;
			mtx[2][1] = 1;
			mtx[2][2] = 1;

			break;
		}
		case EAST_3_3:
		{
			PREDEF_FOLD_BEGIN(3, 3, 1);

			mtx[0][0] = -1;
			mtx[0][1] = 1;
			mtx[0][2] = 1;
			mtx[1][0] = -1;
			mtx[1][1] = -2;
			mtx[1][2] = 1;
			mtx[2][0] = -1;
			mtx[2][1] = 1;
			mtx[2][2] = 1;

			break;
		}
		case WEST_3_3:
		{
			PREDEF_FOLD_BEGIN(3, 3, 1);

			mtx[0][0] = 1;
			mtx[0][1] = 1;
			mtx[0][2] = -1;
			mtx[1][0] = 1;
			mtx[1][1] = -2;
			mtx[1][2] = -1;
			mtx[2][0] = 1;
			mtx[2][1] = 1;
			mtx[2][2] = -1;

			break;
		}
		case NORTH_EAST_3_3:
		{
			PREDEF_FOLD_BEGIN(3, 3, 1);

			mtx[0][0] = 1;
			mtx[0][1] = 1;
			mtx[0][2] = 1;
			mtx[1][0] = -1;
			mtx[1][1] = -2;
			mtx[1][2] = 1;
			mtx[2][0] = -1;
			mtx[2][1] = -1;
			mtx[2][2] = 1;

			break;
		}
		case NORTH_WEST_3_3:
		{
			PREDEF_FOLD_BEGIN(3, 3, 1);

			mtx[0][0] = 1;
			mtx[0][1] = 1;
			mtx[0][2] = 1;
			mtx[1][0] = 1;
			mtx[1][1] = -2;
			mtx[1][2] = -1;
			mtx[2][0] = 1;
			mtx[2][1] = -1;
			mtx[2][2] = -1;

			break;
		}
		case SOUTH_EAST_3_3:
		{
			PREDEF_FOLD_BEGIN(3, 3, 1);

			mtx[0][0] = -1;
			mtx[0][1] = -1;
			mtx[0][2] = 1;
			mtx[1][0] = -1;
			mtx[1][1] = -2;
			mtx[1][2] = 1;
			mtx[2][0] = 1;
			mtx[2][1] = 1;
			mtx[2][2] = 1;

			break;
		}
		case SOUTH_WEST_3_3:
		{
			PREDEF_FOLD_BEGIN(3, 3, 1);

			mtx[0][0] = 1;
			mtx[0][1] = -1;
			mtx[0][2] = -1;
			mtx[1][0] = 1;
			mtx[1][1] = -2;
			mtx[1][2] = -1;
			mtx[2][0] = 1;
			mtx[2][1] = 1;
			mtx[2][2] = 1;

			break;
		}
		default:
			throw string("Указанной предопределенной матрицы не существует");
	}

	return algo::fold(bmp, k, mtx, matrix_height, matrix_width);
}

int double_compare(const void * op1, const void * op2)
{
	if(* (double *) op1 < * (double *) op2)
		return -1;

	if(* (double *) op1 == * (double *) op2)
		return 0;

	return 1;
}

shared_ptr<c_bitmap> algo::median(c_bitmap & bmp, unsigned matrix_height, unsigned matrix_width)
{
	int v, u, from_v, to_v, from_u, to_u;
	unsigned q, sc, tv, tu, height = bmp.aoi().height(), width = bmp.aoi().width();
	unsigned matrix_height_2 = matrix_height >> 1, matrix_width_2 = matrix_width >> 1;
	vector<double> buf[CH_NUM];
	shared_ptr<c_bitmap> new_bmp(new c_bitmap(height, width));

	if(! matrix_height || matrix_height > height || ! matrix_width || matrix_width > width)
		throw string("Некорректные размеры матрицы");

	for(auto & p : buf)
		p.resize(matrix_height * matrix_width);

	for(v = 0; v < height; v++)
		for(u = 0; u < width; u++)
		{
			from_v = v - matrix_height_2;
			to_v = v + matrix_height_2;

			from_u = u - matrix_width_2;
			to_u = u + matrix_width_2;

			if(from_v < 0)
				from_v = 0;

			if(to_v >= height)
				to_v = height - 1;

			if(from_u < 0)
				from_u = 0;

			if(to_u >= width)
				to_u = width - 1;

			for(tv = from_v, sc = 0; tv <= to_v; tv++)
				for(tu = from_u; tu <= to_u; tu++, sc++)
				{
					c_color & px = bmp.pixel(tv, tu);

					for(q = 0; q < CH_NUM; q++)
						buf[q][sc] = px[q];
				}

			for(q = 0; q < CH_NUM; q++)
			{
				sort(buf[q].begin(), buf[q].end());
				new_bmp->pixel(v, u).set(q, buf[q][sc >> 1]);
			}		
		}

	return new_bmp;
}

shared_ptr<c_bitmap> algo::unsharp_masking(c_bitmap & bmp, e_matrix matrix, double k)
{
	if(k < 0 || k >= 1)
		throw string("Некорректное значение множителя");

	unsigned v, u, q, height = bmp.aoi().height(), width = bmp.aoi().width();
	double k_1 = 1 - k;
	shared_ptr<c_bitmap> new_bmp = predef_fold(bmp, matrix);

	for(q = 0; q < CH_NUM; q++)
		for(v = 0; v < height; v++)
			for(u = 0; u < width; u++)
				new_bmp->pixel(v, u).set(q, k * bmp.pixel(v, u)[q] - k_1 * new_bmp->pixel(v, u)[q]);

	return new_bmp;
}

shared_ptr<c_bitmap> algo::differential_operator(c_bitmap & bmp, double k)
{
	unsigned v, u, q, height = bmp.aoi().height(), width = bmp.aoi().width();
	shared_ptr<c_bitmap> new_bmp = predef_fold(bmp, algo::e_matrix::LAPLACIAN_3_3);

	for(q = 0; q < CH_NUM; q++)
		for(v = 0; v < height; v++)
			for(u = 0; u < width; u++)
				new_bmp->pixel(v, u).set(q, bmp.pixel(v, u)[q] + k * new_bmp->pixel(v, u)[q]);

	return new_bmp;
}

