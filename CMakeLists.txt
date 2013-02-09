
# TODO valgrind

cmake_minimum_required(VERSION 2.8)

find_package(SDL REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(PC_PCRECPP QUIET libpcrecpp)

if(NOT CMAKE_BUILD_TYPE)

	#
	# Цель по умолчанию - Debug
	# Для оптимизированной цели: cmake -DCMAKE_BUILD_TYPE=Release ..
	#

	set(CMAKE_BUILD_TYPE Debug)

endif()

set(CMAKE_CXX_FLAGS "-std=c++11")
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

############################################################################ 
# Библиотеки

set(amv_bmp_sources algo/bmp/bmp.cpp)
set(amv_correction_sources algo/correction/correction.cpp)
set(amv_filter_sources algo/filter/filter.cpp)
set(amv_matrix_sources algo/matrix/matrix.cpp)
set(amv_tiff_sources algo/tiff/tiff.cpp)
set(amv_various_sources algo/various/various.cpp)
set(amv_wavelet_sources algo/wavelet/wavelet.cpp)

add_library(amv_bmp SHARED ${amv_bmp_sources})
add_library(amv_correction SHARED ${amv_correction_sources})
add_library(amv_filter SHARED ${amv_filter_sources})
add_library(amv_matrix SHARED ${amv_matrix_sources})
add_library(amv_tiff SHARED ${amv_tiff_sources})
add_library(amv_various SHARED ${amv_various_sources})
add_library(amv_wavelet SHARED ${amv_wavelet_sources})

############################################################################ 
# Исполняемый файл

set(amv_ip_sources main.cpp command.cpp stub.cpp lib/file.cpp bitmap/color.cpp bitmap/bitmap.cpp bitmap/display.cpp algo/interface.cpp)
add_executable(amv_ip ${amv_ip_sources})
target_link_libraries(amv_ip amv_bmp amv_correction amv_filter amv_matrix amv_tiff amv_various amv_wavelet m readline history ${SDL_LIBRARY} ${PC_PCRECPP_LDFLAGS})

