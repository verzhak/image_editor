
#ifndef DISPLAY_H
#define DISPLAY_H

#include "all.h"
#include "bitmap/color.h"
#include "bitmap/bitmap.h"

#define BORDER_WIDTH 2
#define BORDER_WIDTH_2 4
#define BORDER_COLOR 0xFF0000

class c_display
{
	int pid, disp_name_pp[2], sync_pp[2];
	shared_ptr<c_bitmap> bmp;

	void create();
	int thread_main();

	public:

		bool is_full;
		string disp_name;

		c_display(c_bitmap * t_bmp, string t_disp_name);
		c_display(shared_ptr<c_bitmap> t_bmp, string t_disp_name);
		~c_display();

		inline c_bitmap & bitmap() { return (* bmp); };
		c_bitmap * bitmap_ptr() { return bmp.get(); };
		operator shared_ptr<c_bitmap>() { return bmp; };
		void refresh();
		void toggle_full();

};

#endif

