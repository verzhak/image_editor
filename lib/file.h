
#ifndef FILE_H
#define FILE_H

#include "all.h"

namespace file
{
	enum e_mode
	{
		MODE_READ, MODE_WRITE
	};

	class c_file
	{
		bool delete_after_write;
		FILE * fl;
		string fname;
		e_mode mode;

		public:

			c_file(string t_fname, e_mode t_mode);
			~c_file();

			void read(void * pnt, unsigned size);
			void write(const void * pnt, unsigned size);
			long tell();
			void seek_set(long offset);
			inline void mark_as_not_delete_on_close_after_write() { delete_after_write = false; } ; // Ха - ха
	};
}

#endif

