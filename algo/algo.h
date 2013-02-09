
#ifndef ALGO_H
#define ALGO_H

#include "all.h"
#include "lib/file.h"
#include "bitmap/color.h"
#include "bitmap/bitmap.h"
#include "bitmap/display.h"

#define TO_RADIAN(angle) (M_PI * (angle) / 180.0)

using namespace file;

namespace algo
{
	shared_ptr<c_bitmap> load_matrix_16(const string & fname, unsigned height, unsigned width);
	shared_ptr<c_bitmap> load_matrix_256(const string & fname, unsigned height, unsigned width);

	shared_ptr<c_bitmap> load_bmp(const string & fname);
	void save_bmp(c_bitmap & bmp, t_channel ch, const string & fname);

	void load_tiff(const string & fname, vector<shared_ptr<c_bitmap> > & bmp_array);
	void save_tiff(vector<shared_ptr<c_bitmap> > & bmp_array, vector<e_channel_type> & ch_type, vector<t_channel> & ch, const string & fname);

	void get_min_max(c_bitmap & bmp, c_color & min, c_color & max, bool in_aoi = false);
	void contrast(c_bitmap & bmp);
	shared_ptr<c_bitmap> rotate(c_bitmap & bmp, double alpha, double y0, double x0);
	void diff(c_bitmap & bmp_1, c_bitmap & bmp_2);
	shared_ptr<c_bitmap> resize(c_bitmap & bmp, double k);

	enum e_wavelet
	{
		HAAR
	};

	void wavelet_down(c_bitmap & bmp, shared_ptr<c_bitmap> & A, shared_ptr<c_bitmap> & Dh, shared_ptr<c_bitmap> & Dv, shared_ptr<c_bitmap> & Dd, e_wavelet fun);
	shared_ptr<c_bitmap> wavelet_up(shared_ptr<c_bitmap> & A, shared_ptr<c_bitmap> & Dh, shared_ptr<c_bitmap> & Dv, shared_ptr<c_bitmap> & Dd, e_wavelet fun);

	enum e_matrix
	{
		UNKNOWN_MATRIX,

		LOW_FREQ_3_3_1,		// 3 на 3, все единицы, масштабирующий коэффициент = 1 / 9
		LOW_FREQ_3_3_2_1,	// 3 на 3, все единицы, в центре - 2, масштабирующий коэффициент = 1 / 10
		LOW_FREQ_5_5_1,		// 5 на 5, все единицы, масштабирующий коэффициент = 1 / 25
		LOW_FREQ_5_5_2_1,	// 5 на 5, все единицы, в центре - 2, масштабирующий коэффициент = 1 / 26
		LOW_FREQ_7_7_1,		// 7 на 7, все единицы, масштабирующий коэффициент = 1 / 49
		LOW_FREQ_7_7_2_1,	// 7 на 7, все единицы, в центре - 2, масштабирующий коэффициент = 1 / 50

		LAPLACIAN_3_3,		// Лапласиан 3 на 3

		NORTH_3_3,
		SOUTH_3_3,
		EAST_3_3,
		WEST_3_3,
		NORTH_EAST_3_3,
		NORTH_WEST_3_3,
		SOUTH_EAST_3_3,
		SOUTH_WEST_3_3
	};

	shared_ptr<c_bitmap> fold(c_bitmap & bmp, double k, vector<vector<double> > & matrix, unsigned matrix_height, unsigned matrix_width);
	shared_ptr<c_bitmap> predef_fold(c_bitmap & bmp, e_matrix matrix);
	shared_ptr<c_bitmap> median(c_bitmap & bmp, unsigned matrix_height, unsigned matrix_width);
	shared_ptr<c_bitmap> unsharp_masking(c_bitmap & bmp, e_matrix matrix, double k);
	shared_ptr<c_bitmap> differential_operator(c_bitmap & bmp, double k);

	shared_ptr<c_bitmap> radiometric_correction(c_bitmap & bmp, unsigned N);

	extern "C" c_bitmap * morphology(c_bitmap * src);
};

#endif

