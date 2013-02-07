
#include "bitmap/color.h"

const double c_color::max_ch = 1, c_color::min_ch = -1;
const double c_color::size_ch = c_color::max_ch - c_color::min_ch;
const double c_color::at_u8 = c_color::max_ch / 255, c_color::at_u16 = c_color::max_ch / 65535, c_color::at_u32 = 1;
const double c_color::at_s8 = c_color::at_u8 * 2, c_color::at_s16 = c_color::at_u16 * 2, c_color::at_s32 = c_color::at_u32 * 2;
const double c_color::k1_u8 = 255 / c_color::size_ch, c_color::k2_u8 = - 255 * c_color::min_ch / c_color::size_ch;

c_color::c_color(double r, double g, double b)
{
	ch[R] = r;
	ch[G] = g;
	ch[B] = b;

	TO_RANGE(R);
	TO_RANGE(G);
	TO_RANGE(B);
}

#define MEMBER(fname, ret_val, param, do_it) \
ret_val c_color::fname(param)\
{\
	unsigned u;\
\
	for(u = 0; u < CH_NUM; u++)\
	{\
		do_it;\
	}\
}

MEMBER(c_color, , void, ch[u] = 0);
MEMBER(c_color, , double * t_ch, \
	ch[u] = t_ch[u];\
	TO_RANGE(u);\
);

MEMBER(operator=, void, const c_color & color, ch[u] = color.ch[u]);
MEMBER(operator+=, void, const c_color & color, ch[u] += color.ch[u]);
MEMBER(operator-=, void, const c_color & color, ch[u] -= color.ch[u]);
MEMBER(operator*=, void, double val, ch[u] *= val);
MEMBER(operator/=, void, double val, ch[u] /= val);

MEMBER(set_all_u8, void, uint8_t value, set_u8(u, value));
MEMBER(set_all, void, double value, \
	ch[u] = value;\
	TO_RANGE(u);\
);

