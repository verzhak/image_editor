
#include "lib/file.h"

file::c_file::c_file(string t_fname, file::e_mode t_mode) :
	delete_after_write(true), fl(NULL), fname(t_fname), mode(t_mode)
{
	string str_mode;

	switch(mode)
	{
		case file::MODE_READ:
		{
			str_mode = "r";

			break;
		}
		case file::MODE_WRITE:
		{
			str_mode = "w";

			break;
		}
	}

	if(
		(fl = fopen(fname.c_str(), str_mode.c_str())) == NULL
	  )
		throw string("Открыть файл не удалось");
}

file::c_file::~c_file()
{
	if(fl != NULL)
	{
		fclose(fl);

		if(mode == file::MODE_WRITE && delete_after_write)
			remove(fname.c_str());
	}
}

void file::c_file::read(void * pnt, unsigned size)
{
	if(fl == NULL)
		throw string("Файл не был открыт");
	else
		fread(pnt, size, 1, fl);
}

void file::c_file::write(const void * pnt, unsigned size)
{
	if(fl == NULL)
		throw string("Файл не был открыт");
	else
		fwrite(pnt, size, 1, fl);
}

long file::c_file::tell()
{
	return ftell(fl);
}

void file::c_file::seek_set(long offset)
{
	if(fl == NULL)
		throw string("Файл не был открыт");
	else
		fseek(fl, offset, SEEK_SET);
}

