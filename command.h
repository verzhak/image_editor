
#ifndef COMMAND_H
#define COMMAND_H

#include "all.h"
#include "bitmap/color.h"
#include "bitmap/bitmap.h"
#include "bitmap/display.h"
#include "algo/algo.h"

namespace main_loop
{
	enum e_command
	{
		COMMAND_NOTHING, COMMAND_EXIT, COMMAND_EXECUTE,
		COMMAND_LOAD, LOAD_MATRIX, LOAD_BMP, LOAD_TIFF,
		COMMAND_CLOSE, COMMAND_LIST, COMMAND_REFRESH, COMMAND_INFO, COMMAND_RENAME, COMMAND_TOGGLE_FULL_DISP,
		COMMAND_SAVE, SAVE_BMP, SAVE_TIFF,
		COMMAND_SET_AOI, COMMAND_RESET_AOI, COMMAND_DISP_AOI,
		COMMAND_CONTRAST, COMMAND_ROTATE, COMMAND_DIFF, COMMAND_RESIZE,
		COMMAND_WAVELET, COMMAND_WAVELET_DOWN, COMMAND_WAVELET_UP,
		COMMAND_FILE, COMMAND_FILE_DIFF_SIZE,
		COMMAND_FOLD, COMMAND_DIFFERENTIAL_OPERATOR, COMMAND_UNSHARP_MASKING, COMMAND_MEDIAN,
		COMMAND_RADIOMETRIC_CORRECTION,
		COMMAND_MORPHOLOGY
	};

	void init();
	void loop();
	void destroy();
};

#endif

