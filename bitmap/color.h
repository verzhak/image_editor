
#ifndef COLOR_H
#define COLOR_H

#include "all.h"

#define TO_RANGE(c) \
{\
	if(ch[(c)] < c_color::min_ch)\
		ch[(c)] = c_color::min_ch;\
	else if(ch[(c)] > c_color::max_ch)\
		ch[(c)] = c_color::max_ch;\
}

#define SET_INT(c, t, m) \
{\
	ch[(c)] = (m) * (t);\
}

#define GET_UINT(c, m) \
{\
	return ((ch[(c)] > 0) ? (ch[(c)] / (m)) : 0);\
}

#define GET_SINT(c, m) \
{\
	return (ch[(c)] / (m));\
}

enum e_channel_type
{
	CHANNEL_TYPE_U8, CHANNEL_TYPE_U16, CHANNEL_TYPE_U32,
	CHANNEL_TYPE_S8, CHANNEL_TYPE_S16, CHANNEL_TYPE_S32,
	CHANNEL_TYPE_FLOAT, CHANNEL_TYPE_DOUBLE
};

typedef int t_channel;

class c_color
{

#define ALL_CHANNEL -1
#define CH_NUM 3
#define R 0
#define G 1
#define B 2

	double ch[CH_NUM];

	public:

		static const double max_ch, min_ch, size_ch, at_u8, at_u16, at_u32, at_s8, at_s16, at_s32, k1_u8, k2_u8;

		c_color();
		c_color(double * t_ch);
		c_color(double r, double g, double b);

		inline void set_u8(unsigned ch_num, uint8_t value) { SET_INT(ch_num, value, c_color::at_u8); };
		inline void set_u16(unsigned ch_num, uint16_t value) { SET_INT(ch_num, value, c_color::at_u16); };
		inline void set_u32(unsigned ch_num, uint32_t value) { SET_INT(ch_num, value, c_color::at_u32); };
		inline void set_s8(unsigned ch_num, int8_t value) { SET_INT(ch_num, value, c_color::at_s8); };
		inline void set_s16(unsigned ch_num, int16_t value) { SET_INT(ch_num, value, c_color::at_s16); };
		inline void set_s32(unsigned ch_num, int32_t value) { SET_INT(ch_num, value, c_color::at_s32); };
		inline void set_real(unsigned ch_num, double value) { ch[ch_num] = value; TO_RANGE(ch_num); };
		inline void set(unsigned ch_num, double value) { ch[ch_num] = value; TO_RANGE(ch_num); };
		void set_all_u8(uint8_t value);
		void set_all(double value);

		inline uint8_t get_u8(unsigned ch_num) { GET_UINT(ch_num, c_color::at_u8); }
		inline uint16_t get_u16(unsigned ch_num) { GET_UINT(ch_num, c_color::at_u16); }
		inline uint32_t get_u32(unsigned ch_num) { GET_UINT(ch_num, c_color::at_u32); }
		inline int8_t get_s8(unsigned ch_num) { GET_SINT(ch_num, c_color::at_s8); }
		inline int16_t get_s16(unsigned ch_num) { GET_SINT(ch_num, c_color::at_s16); }
		inline int32_t get_s32(unsigned ch_num) { GET_SINT(ch_num, c_color::at_s32); }
		inline double get_real(unsigned ch_num) { return ch[ch_num]; };
		inline double get(unsigned ch_num) { return ch[ch_num]; };
		inline uint8_t get_full_u8(unsigned ch_num) { return (c_color::k1_u8 * ch[ch_num] + c_color::k2_u8); };

		void operator=(const c_color & color);
		void operator+=(const c_color & color);
		void operator-=(const c_color & color);
		void operator*=(double val);
		void operator/=(double val);
		inline double & operator[](unsigned ind) { return ch[ind]; };
};

#endif

