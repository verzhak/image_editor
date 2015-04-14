
#include "command.h"

// TODO Ctrl + D

using namespace main_loop;
using namespace algo;

map<string, c_display *> disp;
map<e_command, RE *> regexp;

void SIG_terminate_handler(int notused)
{
	kill(0, SIGTERM);
}

void my_add_history(char * command)
{
	HIST_ENTRY * h_entry;

	if(command != NULL)
	{
		using_history();

		while(
				(h_entry = previous_history()) != NULL
			 )
		{
			if(! strcmp(h_entry->line, command))
			{
				remove_history(where_history());

				break;
			}
		}

		using_history();
		add_history(command);
		free(command);
	}
	else
		using_history();
}

void parse(queue<string> & list, char * str)
{
	char sym;
	string temp;
	bool not_comment, not_find_first;

	for(not_comment = true, not_find_first = true; (* str) != '\0'; str++)
	{
		switch(* str)
		{
			case ',':
			case '\n':
			{
				if(not_comment)
					list.push(temp);
				else
					not_comment = true;

				temp = "";
				not_find_first = true;

				break;
			}
			default:
			{
				sym = (* str);

				if(not_find_first && ! (sym == ' ' || sym == '\t'))
				{
					not_find_first = false;

					if(sym == '#')
						not_comment = false;
				}

				if(sym == '~')
					temp += getenv("HOME");
				else
					temp += sym;
			}
		}
	}

	if(temp != "")
	{
		list.push(temp);
		temp = "";
	}
}

void script_load(queue<string> & command_list, string & fname)
{
	try
	{
		size_t notused;
		char * buf = NULL;
		FILE * fl = fopen(fname.c_str(), "r");

		if(fl == NULL)
			throw string("Ошибка при открытии файла");

		while(getline(& buf, & notused, fl) != -1)
		{
			parse(command_list, buf);
			free(buf);
			buf = NULL;
		}

		fclose(fl);
	}
	catch(string & err)
	{
		fprintf(stderr, "\n[Исключение] %s\n", err.c_str());
	}
}

#define FIND_DISP(disp_name, p) \
{\
	if(\
		(p_disp_iter = disp.find((disp_name))) == disp.end() \
	  )\
		throw string("Дисплея с указанным именем не существует");\
\
	(p) = p_disp_iter->second;\
}

#define INSERT_DISP(fcall) \
	disp.insert(make_pair(new_disp_name, new c_display(fcall, new_disp_name)));\

#define CHECK_INSERT_DISP(fcall) \
{\
	if(disp.find(new_disp_name) != disp.end())\
		throw string("Дисплей с данным именем уже существует");\
\
	INSERT_DISP(fcall);\
}

#define LOAD_INSERT_DISP_MATRIX(drv_name, marker, msg, fcall)\
{\
	if(driver == drv_name)\
	{\
		if(! regexp[marker]->FullMatch(arg, & height, & width))\
			throw string(msg);\
	\
		new_disp_name = fname;\
		INSERT_DISP(fcall);\
	\
		return true;\
	}\
}

bool do_command(string & command)
{
	unsigned v, y, x, height, width, N, ch;
	double angle, k;
	string driver, channel, fname, arg, operation, disp_name, disp_name_2, new_disp_name, data_type, range, angle_unity;
	string disp_name_A, disp_name_Dh, disp_name_Dv, disp_name_Dd, wv_fun, matrix_name;
	e_matrix matrix;
	c_display * p_disp, * p_disp_2;
	map<string, c_display *>::iterator iter, p_disp_iter;
	StringPiece str_piece;

	try
	{
		if(regexp[COMMAND_LOAD]->FullMatch(command, & driver, & fname, & arg))
		{
			if(disp.find(fname) != disp.end())
				throw string("Файл уже загружен");

			LOAD_INSERT_DISP_MATRIX("matrix_16", LOAD_MATRIX, "Некорректные аргументы при загрузке matrix_16-файла", load_matrix_16(fname, height, width));
			LOAD_INSERT_DISP_MATRIX("matrix_256", LOAD_MATRIX, "Некорректные аргументы при загрузке matrix_256-файла", load_matrix_256(fname, height, width));

			if(driver == "bmp")
			{
				if(! regexp[LOAD_BMP]->FullMatch(arg))
					throw string("Некорректные аргументы при загрузке BMP-файла");

				new_disp_name = fname;
				INSERT_DISP(load_bmp(fname));

				return true;
			}
			
			if(driver == "tiff")
			{
				if(! regexp[LOAD_TIFF]->FullMatch(arg))
					throw string("Некорректные аргументы при загрузке TIFF-файла");

				vector<shared_ptr<c_bitmap> > bmp;

				load_tiff(fname, bmp);
				
				v = 0;

				for(auto bmp_iter = bmp.begin(); bmp_iter != bmp.end(); v ++, bmp_iter++)
				{
					stringstream str_stream;

					str_stream << fname << "_" << v;
					new_disp_name = str_stream.str();
					INSERT_DISP(* bmp_iter);
				}

				return true;
			}
			
			throw string("Неизвестный драйвер");
		}

		if(regexp[COMMAND_CLOSE]->FullMatch(command, & disp_name))
		{
			FIND_DISP(disp_name, p_disp);
			
			disp.erase(p_disp_iter);
			delete p_disp;

			return true;
		}

		if(regexp[COMMAND_LIST]->FullMatch(command))
		{
			printf("\n");

			for(iter = disp.begin(); iter != disp.end(); iter++)
				printf("%s\n", iter->first.c_str());

			printf("\n");

			return true;
		}

		if(regexp[COMMAND_REFRESH]->FullMatch(command))
		{
			for(iter = disp.begin(); iter != disp.end(); iter++)
				iter->second->refresh();

			return true;
		}

		if(regexp[COMMAND_SAVE]->FullMatch(command, & driver, & fname, & arg))
		{
			if(driver == "bmp")
			{
				if(regexp[SAVE_BMP]->FullMatch(arg, & disp_name, & channel))
				{
					FIND_DISP(disp_name, p_disp);

					if(channel == "all")
						ch = ALL_CHANNEL;
					else if(channel == "red")
						ch = R;
					else if(channel == "green")
						ch = G;
					else if(channel == "blue")
						ch = B;
					else
						throw string("Некорректное название канала");

					save_bmp(p_disp->bitmap(), ch, fname);

					return true;
				}

				throw string("Некорректные аргументы команды сохранения в BMP-файл");
			}

			if(driver == "tiff")
			{
				vector<shared_ptr<c_bitmap> > save_bmp;
				vector<e_channel_type> channel_type;
				vector<t_channel> channel_num;

				str_piece.set(arg.c_str());

				while(regexp[SAVE_TIFF]->Consume(& str_piece, & disp_name, & data_type, & channel))
				{
					FIND_DISP(disp_name, p_disp);

					save_bmp.push_back((shared_ptr<c_bitmap>) (* p_disp));

					if(channel == "all")
						channel_num.push_back(ALL_CHANNEL);
					else if(channel == "red")
						channel_num.push_back(R);
					else if(channel == "green")
						channel_num.push_back(G);
					else if(channel == "blue")
						channel_num.push_back(B);

					if(data_type == "u8")
						channel_type.push_back(CHANNEL_TYPE_U8);
					else if(data_type == "u16")
						channel_type.push_back(CHANNEL_TYPE_U16);
					else if(data_type == "u32")
						channel_type.push_back(CHANNEL_TYPE_U32);
					else if(data_type == "s8")
						channel_type.push_back(CHANNEL_TYPE_S8);
					else if(data_type == "s16")
						channel_type.push_back(CHANNEL_TYPE_S16);
					else if(data_type == "s32")
						channel_type.push_back(CHANNEL_TYPE_S32);
					else if(data_type == "float")
						channel_type.push_back(CHANNEL_TYPE_FLOAT);
					else
						channel_type.push_back(CHANNEL_TYPE_DOUBLE);
				}

				if(str_piece != "")
					throw string("Некорректные аргументы команды сохранения в TIFF-файл");

				save_tiff(save_bmp, channel_type, channel_num, fname);

				return true;
			}

			throw string("Неизвестный драйвер");
		}

		if(regexp[COMMAND_INFO]->FullMatch(command, & disp_name))
		{
			FIND_DISP(disp_name, p_disp);
			c_bitmap & bmp = p_disp->bitmap();

			printf("\
\nРазмер дисплея: %u строк, %u столбцов\n\n\
Область интереса:\n\n\tВерхний левый угол = (%u, %u)\n\tРазмер = %u строк, %u столбцов\n\n\
Режим отображения: %s\n\n\
",
				bmp.height, bmp.width,
				bmp.aoi().ul_y(), bmp.aoi().ul_x(), bmp.aoi().height(), bmp.aoi().width(),
				p_disp->is_full ? "[MIN, MAX] -> [0, 255]" : "[0, MAX] -> [0, 255]");

			return true;
		}

		if(regexp[COMMAND_RENAME]->FullMatch(command, & disp_name, & new_disp_name))
		{
			FIND_DISP(disp_name, p_disp);

			if(disp.find(new_disp_name) != disp.end())
				throw string("Дисплей с данным именем уже существует");

			disp.erase(p_disp_iter);
			disp.insert(make_pair(new_disp_name, p_disp));
			p_disp->disp_name = new_disp_name;
			p_disp->refresh();

			return true;
		}

		if(regexp[COMMAND_TOGGLE_FULL_DISP]->FullMatch(command, & disp_name))
		{
			FIND_DISP(disp_name, p_disp);

			p_disp->toggle_full();
			p_disp->refresh();

			return true;
		}

		if(regexp[COMMAND_SET_AOI]->FullMatch(command, & disp_name, & y, & x, & height, & width))
		{
			FIND_DISP(disp_name, p_disp);

			p_disp->bitmap().aoi().reset(y, x, height, width);
			p_disp->refresh();

			return true;
		}

		if(regexp[COMMAND_RESET_AOI]->FullMatch(command, & disp_name))
		{
			FIND_DISP(disp_name, p_disp);

			p_disp->bitmap().aoi().reset();
			p_disp->refresh();

			return true;
		}

		if(regexp[COMMAND_DISP_AOI]->FullMatch(command, & disp_name, & new_disp_name))
		{
			FIND_DISP(disp_name, p_disp);
			CHECK_INSERT_DISP(shared_ptr<c_bitmap>(new c_bitmap(p_disp->bitmap())));

			return true;
		}

		if(regexp[COMMAND_CONTRAST]->FullMatch(command, & disp_name))
		{
			FIND_DISP(disp_name, p_disp);

			contrast(p_disp->bitmap());
			p_disp->refresh();

			return true;
		}

		if(regexp[COMMAND_ROTATE]->FullMatch(command, & disp_name, & new_disp_name, & angle, & angle_unity, & y, & x))
 		{
			FIND_DISP(disp_name, p_disp);
 
			if(y >= p_disp->bitmap().height || x >= p_disp->bitmap().width)
				throw string("Одна или обе координаты выходят за границы изображения");
 
			if(angle_unity == "g")
				angle = TO_RADIAN(angle);
 
			CHECK_INSERT_DISP(rotate(p_disp->bitmap(), angle, y, x));
 
			return true;
		}

		if(regexp[COMMAND_DIFF]->FullMatch(command, & disp_name, & disp_name_2))
		{
			FIND_DISP(disp_name, p_disp);
			FIND_DISP(disp_name_2, p_disp_2);

			diff(p_disp->bitmap(), p_disp_2->bitmap());

			return true;
		}

		if(regexp[COMMAND_RESIZE]->FullMatch(command, & disp_name, & new_disp_name, & k))
 		{
			FIND_DISP(disp_name, p_disp);
			CHECK_INSERT_DISP(resize(p_disp->bitmap(), k));
 
			return true;
		}

		if(regexp[COMMAND_WAVELET]->FullMatch(command, & arg))
		{
			shared_ptr<c_bitmap> p_A, p_Dh, p_Dv, p_Dd, res;
			c_display * p_disp_A, * p_disp_Dh, * p_disp_Dv, * p_disp_Dd;
			string new_disp_name_A, new_disp_name_Dh, new_disp_name_Dv, new_disp_name_Dd;

			if(regexp[COMMAND_WAVELET_DOWN]->FullMatch(arg, & wv_fun, & disp_name))
			{
				FIND_DISP(disp_name, p_disp);

				if(wv_fun == "haar")
					wavelet_down(p_disp->bitmap(), p_A, p_Dh, p_Dv, p_Dd, e_wavelet::HAAR);
				else
					throw string("Неизвестный тип вейвлет-преобразования");

				new_disp_name_A = disp_name + "_A";
				new_disp_name_Dh = disp_name + "_Dh";
				new_disp_name_Dv = disp_name + "_Dv";
				new_disp_name_Dd = disp_name + "_Dd";

				if(
					disp.find(new_disp_name_A) != disp.end()
					||
					disp.find(new_disp_name_Dh) != disp.end()
					||
					disp.find(new_disp_name_Dv) != disp.end()
					||
					disp.find(new_disp_name_Dd) != disp.end()
				  )
					throw string("Вейвлет-преобразование уже было выполнено");

				disp.insert(make_pair(new_disp_name_A, new c_display(p_A, new_disp_name_A)));
				disp.insert(make_pair(new_disp_name_Dh, new c_display(p_Dh, new_disp_name_Dh)));
				disp.insert(make_pair(new_disp_name_Dv, new c_display(p_Dv, new_disp_name_Dv)));
				disp.insert(make_pair(new_disp_name_Dd, new c_display(p_Dd, new_disp_name_Dd)));

				return true;
			}

			if(regexp[COMMAND_WAVELET_UP]->FullMatch(arg, & wv_fun, & disp_name, & disp_name_A, & disp_name_Dh, & disp_name_Dv, & disp_name_Dd))
			{

#define WV_FIND_DISP(disp_name, p_disp, p_bmp)\
{\
	if((disp_name) == "null")\
	{\
		(p_disp) = NULL;\
	}\
	else\
	{\
		FIND_DISP((disp_name), (p_disp));\
		(p_bmp) = * (p_disp);\
	}\
}

				if(disp.find(disp_name) != disp.end())
					throw string("Дисплей с таковым именем уже существует");

				WV_FIND_DISP(disp_name_A, p_disp_A, p_A);
				WV_FIND_DISP(disp_name_Dh, p_disp_Dh, p_Dh);
				WV_FIND_DISP(disp_name_Dv, p_disp_Dv, p_Dv);
				WV_FIND_DISP(disp_name_Dd, p_disp_Dd, p_Dd);

				if(p_disp_A == NULL && p_disp_Dh == NULL && p_disp_Dv == NULL && p_disp_Dd == NULL)
					throw string("Не используется ни одного изображения");

				if(wv_fun == "haar")
					res = wavelet_up(p_A, p_Dh, p_Dv, p_Dd, e_wavelet::HAAR);
				else
					throw string("Неизвестный тип вейвлет-преобразования");

				disp.insert(make_pair(disp_name, new c_display(res, disp_name)));

				return true;
			}

			throw string("Неизвестное действие по вейвлет-обработке изображения (изображений)");
		}

		if(regexp[COMMAND_FILE]->FullMatch(command, & arg))
		{
			string fname_1, fname_2;

			if(regexp[COMMAND_FILE_DIFF_SIZE]->FullMatch(arg, & fname_1, & fname_2))
			{
				struct stat stat_1, stat_2;
				unsigned size_1, size_2;

				if(stat(fname_1.c_str(), & stat_1) || stat(fname_2.c_str(), & stat_2))
					throw string("Ошибка при получении размеров файлов");

				size_1 = stat_1.st_size;
				size_2 = stat_2.st_size;

				printf("\n\nРазмеры файлов:\n\n\t%s: %u байт = %lf килобайт = %lf мегабайт\n\t%s: %u байт = %lf килобайт = %lf мегабайт \
(%lf %% от размера файла %s)\n\n",
						fname_1.c_str(), size_1, size_1 / 1024.0, size_1 / 1048576.0,
						fname_2.c_str(), size_2, size_2 / 1024.0, size_2 / 1048576.0,
						size_2 / (size_1 / 100.0), fname_1.c_str());

				return true;
			}

			throw string("Неизвестное действие по обработке файлов");
		}

		if(regexp[COMMAND_FOLD]->FullMatch(command, & disp_name, & new_disp_name, & matrix_name))
 		{
			FIND_DISP(disp_name, p_disp);

			matrix = e_matrix::UNKNOWN_MATRIX;

			if(matrix_name == "LOW_FREQ_3_3_1")
				matrix = e_matrix::LOW_FREQ_3_3_1;

			if(matrix_name == "LOW_FREQ_3_3_2_1")
				matrix = e_matrix::LOW_FREQ_3_3_2_1;

			if(matrix_name == "LOW_FREQ_5_5_1")
				matrix = e_matrix::LOW_FREQ_5_5_1;

			if(matrix_name == "LOW_FREQ_5_5_2_1")
				matrix = e_matrix::LOW_FREQ_5_5_2_1;

			if(matrix_name == "LOW_FREQ_7_7_1")
				matrix = e_matrix::LOW_FREQ_7_7_1;

			if(matrix_name == "LOW_FREQ_7_7_2_1")
				matrix = e_matrix::LOW_FREQ_7_7_2_1;

			if(matrix_name == "LAPLACIAN_3_3")
				matrix = e_matrix::LAPLACIAN_3_3;

			if(matrix_name == "NORTH_3_3")
				matrix = e_matrix::NORTH_3_3;

			if(matrix_name == "SOUTH_3_3")
				matrix = e_matrix::SOUTH_3_3;

			if(matrix_name == "EAST_3_3")
				matrix = e_matrix::EAST_3_3;

			if(matrix_name == "WEST_3_3")
				matrix = e_matrix::WEST_3_3;

			if(matrix_name == "NORTH_EAST_3_3")
				matrix = e_matrix::NORTH_EAST_3_3;

			if(matrix_name == "NORTH_WEST_3_3")
				matrix = e_matrix::NORTH_WEST_3_3;

			if(matrix_name == "SOUTH_EAST_3_3")
				matrix = e_matrix::SOUTH_EAST_3_3;

			if(matrix_name == "SOUTH_WEST_3_3")
				matrix = e_matrix::SOUTH_WEST_3_3;

			if(matrix == e_matrix::UNKNOWN_MATRIX)
				throw string("Неизвестная матрица свертки");

			CHECK_INSERT_DISP(predef_fold(p_disp->bitmap(), matrix));
 
			return true;
		}

		if(regexp[COMMAND_DIFFERENTIAL_OPERATOR]->FullMatch(command, & disp_name, & new_disp_name, & k))
 		{
			FIND_DISP(disp_name, p_disp);
			CHECK_INSERT_DISP(differential_operator(p_disp->bitmap(), k));
 
			return true;
		}

		if(regexp[COMMAND_UNSHARP_MASKING]->FullMatch(command, & disp_name, & new_disp_name, & matrix_name, & k))
 		{
			FIND_DISP(disp_name, p_disp);
 
			matrix = e_matrix::UNKNOWN_MATRIX;
 
			if(matrix_name == "LOW_FREQ_3_3_1")
				matrix = e_matrix::LOW_FREQ_3_3_1;

			if(matrix_name == "LOW_FREQ_3_3_2_1")
				matrix = e_matrix::LOW_FREQ_3_3_2_1;

			if(matrix_name == "LOW_FREQ_5_5_1")
				matrix = e_matrix::LOW_FREQ_5_5_1;

			if(matrix_name == "LOW_FREQ_5_5_2_1")
				matrix = e_matrix::LOW_FREQ_5_5_2_1;

			if(matrix_name == "LOW_FREQ_7_7_1")
				matrix = e_matrix::LOW_FREQ_7_7_1;

			if(matrix_name == "LOW_FREQ_7_7_2_1")
				matrix = e_matrix::LOW_FREQ_7_7_2_1;

			if(matrix == e_matrix::UNKNOWN_MATRIX)
				throw string("Некорректная матрица свертки");

			CHECK_INSERT_DISP(unsharp_masking(p_disp->bitmap(), matrix, k));
 
			return true;
		}

		if(regexp[COMMAND_MEDIAN]->FullMatch(command, & disp_name, & new_disp_name, & height, & width))
 		{
			FIND_DISP(disp_name, p_disp);
			CHECK_INSERT_DISP(median(p_disp->bitmap(), height, width));
 
			return true;
		}

		if(regexp[COMMAND_RADIOMETRIC_CORRECTION]->FullMatch(command, & disp_name, & new_disp_name, & N))
 		{
			FIND_DISP(disp_name, p_disp);
			CHECK_INSERT_DISP(radiometric_correction(p_disp->bitmap(), N));
 
			return true;
		}

		if(regexp[COMMAND_MORPHOLOGY]->FullMatch(command, & operation, & disp_name, & new_disp_name, & height, & width))
 		{
			FIND_DISP(disp_name, p_disp);

			if(height == 0 || width == 0)
				throw string("Некорректный размер структурного элемента для морфологической операции");

#define MORPHOLOGY_OPERATION(oname, op)\
if(operation == oname)\
{\
	CHECK_INSERT_DISP(op(p_disp->bitmap_ptr(), height, width));\
\
	return true;\
}

//			MORPHOLOGY_OPERATION("dilate", dilate);
//			MORPHOLOGY_OPERATION("erode", erode);
//			MORPHOLOGY_OPERATION("open", morphology_open);
//			MORPHOLOGY_OPERATION("close", morphology_close);
 
			throw string("Неизвестная морфологическая операция");
		}
	}
	catch(string & err)
	{
		fprintf(stderr, "\n[Исключение] Ошибка при выполнении команды - %s\n\n", err.c_str());

		return false;
	}

	fprintf(stderr, "\nНеизвестная команда\n\n");

	return false;
}

// ############################################################################ 

void main_loop::init()
{
	signal(SIGINT, & SIG_terminate_handler); // Ctrl + C

	regexp.insert(make_pair(COMMAND_NOTHING, new RE("^[ \\t\\n]*$")));
	regexp.insert(make_pair(COMMAND_EXIT, new RE("^[ \\t]*(?:exit|e)[ \\t]*$")));
	regexp.insert(make_pair(COMMAND_EXECUTE, new RE("^[ \\t]*(?:execute|exec)[ \\t]+([^ \\t]+)[ \\t]*$")));

	regexp.insert(make_pair(COMMAND_LOAD, new RE("^[ \\t]*(?:load|l)[ \\t]+([^ \\t]+)[ \\t]+([^ \\t]+)[ \\t]*(.*)$")));
	regexp.insert(make_pair(LOAD_MATRIX, new RE("^[ \\t]*(\\d+)[ \\t]+(\\d+)[ \\t]*$")));
	regexp.insert(make_pair(LOAD_BMP, new RE("^[ \\t]*$")));
	regexp.insert(make_pair(LOAD_TIFF, new RE("^[ \\t]*$")));
	regexp.insert(make_pair(COMMAND_CLOSE, new RE("^[ \\t]*(?:close|c)[ \\t]+([^ \\t]+)[ \\t]*$")));
	regexp.insert(make_pair(COMMAND_LIST, new RE("^[ \\t]*(?:list|ls)[ \\t]*$")));
	regexp.insert(make_pair(COMMAND_REFRESH, new RE("^[ \\t]*(?:refresh|r)[ \\t]*$")));
	regexp.insert(make_pair(COMMAND_SAVE, new RE("^[ \\t]*(?:save|s)[ \\t]+(\\w+)[ \\t]+([^ \\t]+)[ \\t]+([^ \\t].+[^ \\t])[ \\t]*$")));
	regexp.insert(make_pair(SAVE_BMP, new RE("^[ \\t]*([^ \\t]+)[ \\t]+u8[ \\t]+(all|red|green|blue)$")));
	regexp.insert(make_pair(SAVE_TIFF, new RE("[ \\t]*([^ \\t]+)[ \\t]+(u8|u16|u32|s8|s16|s32|float|double)[ \\t]+(all|red|green|blue)")));
	regexp.insert(make_pair(COMMAND_INFO, new RE("^[ \\t]*(?:info|i)[ \\t]+([^ \\t]+)[ \\t]*$")));
	regexp.insert(make_pair(COMMAND_RENAME, new RE("^[ \\t]*(?:rename|rn)[ \\t]+([^ \\t]+)[ \\t]+([^ \\t]+)[ \\t]*$")));
	regexp.insert(make_pair(COMMAND_TOGGLE_FULL_DISP, new RE("^[ \\t]*(?:toggle full|tf)[ \\t]+([^ \\t]+)[ \\t]*$")));

	regexp.insert(make_pair(COMMAND_SET_AOI,
				new RE("^[ \\t]*(?:set aoi|sa)[ \\t]+([^ \\t]+)[ \\t]+(\\d+)[ \\t]+(\\d+)[ \\t]+(\\d+)[ \\t]+(\\d+)[ \\t]*$")));
	regexp.insert(make_pair(COMMAND_RESET_AOI, new RE("^[ \\t]*(?:reset aoi|ra)[ \\t]+([^ \\t]+)[ \\t]*$")));
	regexp.insert(make_pair(COMMAND_DISP_AOI, new RE("^[ \\t]*(?:disp aoi|da)[ \\t]+([^ \\t]+)[ \\t]+(\\w+)[ \\t]*$")));
	
	regexp.insert(make_pair(COMMAND_CONTRAST, new RE("^[ \\t]*(?:contrast|cn)[ \\t]+([^ \\t]+)[ \\t]*$")));
	regexp.insert(make_pair(COMMAND_ROTATE,
				new RE("^[ \\t]*(?:rotate|rt)[ \\t]+([^ \\t]+)[ \\t]+([^ \\t]+)[ \\t]+([-+]?\\d+\\.?\\d*)(r|g)[ \\t]+([\\d]+)[ \\t]+([\\d]+)[ \\t]*$")));
	regexp.insert(make_pair(COMMAND_DIFF, new RE("^[ \\t]*(?:diff|df)[ \\t]+([^ \\t]+)[ \\t]+([^ \\t]+)[ \\t]*$")));
	regexp.insert(make_pair(COMMAND_RESIZE, new RE("^[ \\t]*(?:resize|rs)[ \\t]+([^ \\t]+)[ \\t]+([^ \\t]+)[ \\t]+(\\d+\\.?\\d*)[ \\t]*$")));

	regexp.insert(make_pair(COMMAND_WAVELET, new RE("^[ \\t]*(?:wavelet|wv)[ \\t]+([^ \\t].+)$")));
	regexp.insert(make_pair(COMMAND_WAVELET_DOWN, new RE("down[ \\t]+(haar)[ \\t]+([^ \\t]+)[ \\t]*$")));
	regexp.insert(make_pair(COMMAND_WAVELET_UP, new RE("up[ \\t]+(haar)[ \\t]+([^ \\t]+)[ \\t]+([^ \\t]+)[ \\t]+([^ \\t]+)[ \\t]+([^ \\t]+)[ \\t]+([^ \\t]+)[ \\t]*$")));

	regexp.insert(make_pair(COMMAND_FILE, new RE("^[ \\t]*(?:file|fl)[ \\t]+([^ \\t].+)$")));
	regexp.insert(make_pair(COMMAND_FILE_DIFF_SIZE, new RE("diff[ \\t]+size[ \\t]+([^ \\t]+)[ \\t]+([^ \\t]+)[ \\t]*$")));

	regexp.insert(make_pair(COMMAND_FOLD, new RE("^[ \\t]*(?:fold|f)[ \\t]+([^ \\t]+)[ \\t]+([^ \\t]+)[ \\t]+([^ \\t]+)[ \\t]*$")));
	regexp.insert(make_pair(COMMAND_DIFFERENTIAL_OPERATOR,
				new RE("^[ \\t]*(?:diff operator|dfop)[ \\t]+([^ \\t]+)[ \\t]+([^ \\t]+)[ \\t]+([-+]?\\d+\\.?\\d*)[ \\t]*$")));
	regexp.insert(make_pair(COMMAND_UNSHARP_MASKING,
				new RE("^[ \\t]*(?:unsharp masking|unma)[ \\t]+([^ \\t]+)[ \\t]+([^ \\t]+)[ \\t]+([^ \\t]+)[ \\t]+([-+]?\\d+\\.?\\d*)[ \\t]*$")));
	regexp.insert(make_pair(COMMAND_MEDIAN,
				new RE("^[ \\t]*(?:median|med)[ \\t]+([^ \\t]+)[ \\t]+([^ \\t]+)[ \\t]+([\\d]+)[ \\t]+([\\d]+)[ \\t]*$")));

	regexp.insert(make_pair(COMMAND_RADIOMETRIC_CORRECTION,
				new RE("^[ \\t]*(?:correction radiometric|crad)[ \\t]+([^ \\t]+)[ \\t]+([^ \\t]+)[ \\t]+([\\d]+)[ \\t]*$")));

	regexp.insert(make_pair(COMMAND_MORPHOLOGY,
				new RE("^[ \\t]*(?:morphology|m)[ \\t]+(\\w+)[ \\t]+([^ \\t]+)[ \\t]+([^ \\t]+)[ \\t]+([\\d]+)[ \\t]+([\\d]+)[ \\t]*$")));
}

void main_loop::destroy()
{
	map<string, c_display *>::iterator disp_iter;
	map<e_command, RE *>::iterator regexp_iter;

	for(disp_iter = disp.begin(); disp_iter != disp.end(); disp_iter++)
		delete disp_iter->second;

	for(regexp_iter = regexp.begin(); regexp_iter != regexp.end(); regexp_iter++)
		delete regexp_iter->second;
}

void main_loop::loop()
{
	bool is_run = true;
	char * command = NULL;
	string fname, next_command;
	queue<string> command_list;

	while(is_run)
	{
		my_add_history(command);

		command = readline(">> ");
		command_list = queue<string>();
		parse(command_list, command);

		while(command_list.size())
		{
			next_command = command_list.front();
			command_list.pop();

			if(regexp[COMMAND_NOTHING]->FullMatch(next_command))
				continue;

			if(regexp[COMMAND_EXIT]->FullMatch(next_command))
			{
				is_run = false;

				continue;
			}

			if(regexp[COMMAND_EXECUTE]->FullMatch(next_command, & fname))
			{
				script_load(command_list, fname);

				continue;
			}

			if(! do_command(next_command))
				break;
		}
	}

	if(command != NULL)
		free(command);
}

