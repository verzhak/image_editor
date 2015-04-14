
# TODO valgrind
# TODO haskell

cmake_minimum_required(VERSION 2.8)

find_package(SDL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(PC_PCRECPP QUIET libpcrecpp)
# set(haskell_include /usr/lib/ghc-7.8.4/include/)
# set(haskell_library /usr/lib/ghc-7.8.4/base-4.7.0.2/libHSbase-4.7.0.2-ghc7.8.4.so /usr/lib/ghc-7.8.4/integer-gmp-0.5.0.0/libHSinteger-gmp-0.5.0.0-ghc7.8.4.so gmp /usr/lib/ghc-7.8.4/ghc-prim-0.3.0.0/libHSghc-prim-0.3.0.0-ghc7.8.4.so /usr/lib/ghc-7.8.4/libHSrts_thr-ghc7.8.4.so rt)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")

	#
	# Цель по умолчанию - Debug
	# Для оптимизированной цели: cmake -DCMAKE_BUILD_TYPE=Release ..
	#

	set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)

endif()

set(CMAKE_CXX_FLAGS "-std=c++11")
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}) # ${haskell_include})
link_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

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

set(amv_ip_sources main.cpp command.cpp stub.cpp lib/file.cpp bitmap/color.cpp bitmap/bitmap.cpp bitmap/display.cpp algo/interface/interface.cpp)
add_executable(amv_ip ${amv_ip_sources})
target_link_libraries(amv_ip amv_bmp amv_correction amv_filter amv_matrix amv_tiff amv_various amv_wavelet m readline history ${SDL_LIBRARY} ${PC_PCRECPP_LDFLAGS}) # amv_morphology ${haskell_library})

############################################################################ 
# Модули на языках, не привязываемых к CMake

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")

	set(amv_morphology_target debug)

else()

	set(amv_morphology_target release)

endif()

# add_custom_target(amv_morphology)
# add_custom_command(TARGET amv_morphology COMMAND make ARGS -C ${CMAKE_CURRENT_SOURCE_DIR}/algo/morphology ${amv_morphology_target})
# add_dependencies(amv_ip amv_morphology)

