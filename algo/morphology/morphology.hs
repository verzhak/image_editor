
{-# LANGUAGE ForeignFunctionInterface #-}

module Morphology where

import Interface
import Foreign
import Foreign.Ptr()
import Foreign.C.Types

-- TODO не прямоугольные структурные элементы
-- TODO прочие операции

base_operation :: ( [ CDouble ] -> CDouble ) -> Bitmap -> Bitmap -> CUInt -> CUInt -> IO Bitmap
base_operation op dst src elem_height elem_width =
	let
	{
		h = height src;
		w = width src;
		e_h_2 = div (fromIntegral elem_height :: Integer) 2;
		e_w_2 = div (fromIntegral elem_width :: Integer) 2;

		max_y = fromIntegral (h - 1) :: Integer;
		max_x = fromIntegral (w - 1) :: Integer;

		pixel y x =
			let
			{
				from_y = maximum [y - e_h_2, 0];
				from_x = maximum [x - e_w_2, 0];
				to_y = minimum [y + e_h_2, max_y];
				to_x = minimum [x + e_w_2, max_x];

				channel_transform (-1) = return ();
				channel_transform ch =
					let
					{
						tch = fromIntegral ch :: CUInt;
						value = op [ get src (fromIntegral ty) (fromIntegral tx) tch | ty <- [ from_y .. to_y ], tx <- [ from_x .. to_x ] ]
					}
					in
					do
					{
						set dst (fromIntegral y) (fromIntegral x) tch value;
						channel_transform (ch - 1)
					}
			}
			in
				channel_transform 2;

		yx_loop :: Integer -> (Integer -> IO ()) -> IO ();
		yx_loop (-1) _ = return ();
		yx_loop val fn =
			do
			{
				fn val;
				yx_loop (val - 1) fn
			};

		aw y = yx_loop max_x (pixel y);
		ah = yx_loop max_y aw
	}
	in
	do
	{
		ah;
		return dst
	}

base_dilate, base_erode :: Bitmap -> Bitmap -> CUInt -> CUInt -> IO Bitmap
base_dilate = base_operation maximum
base_erode = base_operation minimum

operation :: (Bitmap -> Bitmap -> CUInt -> CUInt -> IO Bitmap) -> Bitmap -> CUInt -> CUInt -> IO Bitmap
operation op src =
	let
		dst = create (height src) (width src)
	in
	do
		op dst src

der_operation :: (Bitmap -> Bitmap -> CUInt -> CUInt -> IO Bitmap) -> (Bitmap -> Bitmap -> CUInt -> CUInt -> IO Bitmap) -> Bitmap -> CUInt -> CUInt -> IO Bitmap
der_operation first second src elem_height elem_width =
	let
		dst = create (height src) (width src)
	in
	do
	{
		first dst src elem_height elem_width;
		second dst src elem_height elem_width;
		return dst
	}

--- ############################################################################ 

foreign export ccall dilate :: Bitmap -> CUInt -> CUInt -> IO Bitmap
foreign export ccall erode :: Bitmap -> CUInt -> CUInt -> IO Bitmap
foreign export ccall morphology_open :: Bitmap -> CUInt -> CUInt -> IO Bitmap
foreign export ccall morphology_close :: Bitmap -> CUInt -> CUInt -> IO Bitmap

dilate, erode, morphology_open, morphology_close :: Bitmap -> CUInt -> CUInt -> IO Bitmap

dilate = operation base_dilate
erode = operation base_erode
morphology_open = der_operation base_erode base_dilate
morphology_close = der_operation base_dilate base_erode
