
# TODO cmake
# TODO valgrind

CURRENT_DIR = $(shell pwd)/

OUTPUT_DIR = $(CURRENT_DIR)/build
FNAME = amv_ip
RELEASE_OUTPUT_FNAME = $(OUTPUT_DIR)/$(FNAME)
DEBUG_OUTPUT_FNAME = $(RELEASE_OUTPUT_FNAME)_debug

SRC = main.cpp command.cpp stub.cpp \
	  bitmap/color.cpp bitmap/bitmap.cpp bitmap/display.cpp \
	  algo/interface.cpp

PREFIX=/usr/local

FLAGS = -pipe -std=gnu++0x -fPIC -Wall -Wno-pointer-arith
FLAGS_SHARED = -shared
FLAGS_DEBUG = -g
FLAGS_RELEASE = -O2

INCLUDE = -I$(CURRENT_DIR)

LIB = -lm -lreadline -lhistory $(shell pcre-config --libs) -lpcrecpp $(shell sdl-config --libs)

ALGO_LIB = -L$(OUTPUT_DIR)/ -lamv_bmp -lamv_correction -lamv_filter -lamv_matrix -lamv_tiff -lamv_various -lamv_wavelet

############################################################################ 

debug: clean bmp_debug correction_debug filter_debug matrix_debug tiff_debug various_debug wavelet_debug program_debug
release: clean tiff_release bmp_release correction_release filter_release matrix_release wavelet_release various_release program_release

install:

	mkdir -p $(PREFIX)
	install $(OUTPUT_DIR)/$(FNAME)* $(PREFIX)/bin/$(FNAME)
	install $(OUTPUT_DIR)/libamv*so $(PREFIX)/lib/
	ldconfig

uninstall:

	rm -f $(PREFIX)/bin/$(FNAME)
	rm -f $(PREFIX)/lib/libamv*so
	rmdir -p --ignore-fail-on-non-empty $(PREFIX)/bin $(PREFIX)/lib $(PREFIX)

############################################################################ 

program_debug:

	g++ $(FLAGS) $(FLAGS_DEBUG) $(LIB) $(ALGO_LIB) $(INCLUDE) $(SRC) -o $(DEBUG_OUTPUT_FNAME)

program_release:

	g++ $(FLAGS) $(FLAGS_RELEASE) $(LIB) $(ALGO_LIB) $(INCLUDE) $(SRC) -o $(RELEASE_OUTPUT_FNAME)

############################################################################ 

tiff_debug:
	
	make -C algo/tiff FLAGS="$(FLAGS)" FLAGS_SHARED="$(FLAGS_SHARED)" FLAGS_DEBUG="$(FLAGS_DEBUG)" LIB="$(LIB)" INCLUDE="$(INCLUDE)" \
		OUTPUT_FNAME="$(OUTPUT_DIR)/libamv_tiff.so" debug

bmp_debug:

	make -C algo/bmp FLAGS="$(FLAGS)" FLAGS_SHARED="$(FLAGS_SHARED)" FLAGS_DEBUG="$(FLAGS_DEBUG)" LIB="$(LIB)" INCLUDE="$(INCLUDE)" \
		OUTPUT_FNAME="$(OUTPUT_DIR)/libamv_bmp.so" debug

correction_debug:

	make -C algo/correction FLAGS="$(FLAGS)" FLAGS_SHARED="$(FLAGS_SHARED)" FLAGS_DEBUG="$(FLAGS_DEBUG)" LIB="$(LIB)" INCLUDE="$(INCLUDE)" \
		OUTPUT_FNAME="$(OUTPUT_DIR)/libamv_correction.so" debug

filter_debug:

	make -C algo/filter FLAGS="$(FLAGS)" FLAGS_SHARED="$(FLAGS_SHARED)" FLAGS_DEBUG="$(FLAGS_DEBUG)" LIB="$(LIB)" INCLUDE="$(INCLUDE)" \
		OUTPUT_FNAME="$(OUTPUT_DIR)/libamv_filter.so" debug
	
matrix_debug:

	make -C algo/matrix FLAGS="$(FLAGS)" FLAGS_SHARED="$(FLAGS_SHARED)" FLAGS_DEBUG="$(FLAGS_DEBUG)" LIB="$(LIB)" INCLUDE="$(INCLUDE)" \
		OUTPUT_FNAME="$(OUTPUT_DIR)/libamv_matrix.so" debug
	
wavelet_debug:
	
	make -C algo/wavelet FLAGS="$(FLAGS)" FLAGS_SHARED="$(FLAGS_SHARED)" FLAGS_DEBUG="$(FLAGS_DEBUG)" LIB="$(LIB)" INCLUDE="$(INCLUDE)" \
		OUTPUT_FNAME="$(OUTPUT_DIR)/libamv_wavelet.so" debug

various_debug:

	make -C algo/various FLAGS="$(FLAGS)" FLAGS_SHARED="$(FLAGS_SHARED)" FLAGS_DEBUG="$(FLAGS_DEBUG)" LIB="$(LIB)" INCLUDE="$(INCLUDE)" \
		OUTPUT_FNAME="$(OUTPUT_DIR)/libamv_various.so" debug
	
############################################################################

tiff_release:
	
	make -C algo/tiff FLAGS="$(FLAGS)" FLAGS_SHARED="$(FLAGS_SHARED)" FLAGS_RELEASE="$(FLAGS_RELEASE)" LIB="$(LIB)" INCLUDE="$(INCLUDE)" \
		OUTPUT_FNAME="$(OUTPUT_DIR)/libamv_tiff.so" release
	
bmp_release:

	make -C algo/bmp FLAGS="$(FLAGS)" FLAGS_SHARED="$(FLAGS_SHARED)" FLAGS_RELEASE="$(FLAGS_RELEASE)" LIB="$(LIB)" INCLUDE="$(INCLUDE)" \
		OUTPUT_FNAME="$(OUTPUT_DIR)/libamv_bmp.so" release

correction_release:

	make -C algo/correction FLAGS="$(FLAGS)" FLAGS_SHARED="$(FLAGS_SHARED)" FLAGS_RELEASE="$(FLAGS_RELEASE)" LIB="$(LIB)" INCLUDE="$(INCLUDE)" \
		OUTPUT_FNAME="$(OUTPUT_DIR)/libamv_correction.so" release

filter_release:

	make -C algo/filter FLAGS="$(FLAGS)" FLAGS_SHARED="$(FLAGS_SHARED)" FLAGS_RELEASE="$(FLAGS_RELEASE)" LIB="$(LIB)" INCLUDE="$(INCLUDE)" \
		OUTPUT_FNAME="$(OUTPUT_DIR)/libamv_filter.so" release
	
matrix_release:

	make -C algo/matrix FLAGS="$(FLAGS)" FLAGS_SHARED="$(FLAGS_SHARED)" FLAGS_RELEASE="$(FLAGS_RELEASE)" LIB="$(LIB)" INCLUDE="$(INCLUDE)" \
		OUTPUT_FNAME="$(OUTPUT_DIR)/libamv_matrix.so" release
	
wavelet_release:
	
	make -C algo/wavelet FLAGS="$(FLAGS)" FLAGS_SHARED="$(FLAGS_SHARED)" FLAGS_RELEASE="$(FLAGS_RELEASE)" LIB="$(LIB)" INCLUDE="$(INCLUDE)" \
		OUTPUT_FNAME="$(OUTPUT_DIR)/libamv_wavelet.so" release

various_release:

	make -C algo/various FLAGS="$(FLAGS)" FLAGS_SHARED="$(FLAGS_SHARED)" FLAGS_RELEASE="$(FLAGS_RELEASE)" LIB="$(LIB)" INCLUDE="$(INCLUDE)" \
		OUTPUT_FNAME="$(OUTPUT_DIR)/libamv_various.so" release
	
############################################################################ 

clean:

	rm -f $(OUTPUT_DIR)/*
	mkdir -p $(OUTPUT_DIR)/

distclean:

	rm -Rf $(OUTPUT_DIR)

