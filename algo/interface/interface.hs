
{-# LANGUAGE ForeignFunctionInterface #-}

module Interface where

import Foreign
import Foreign.Ptr()
import Foreign.C.Types

type Bitmap = StablePtr ()

foreign import ccall safe "interface.h"
	create :: CUInt -> CUInt -> Bitmap

foreign import ccall safe "interface.h"
	get :: Bitmap -> CUInt -> CUInt -> CUInt -> CDouble

foreign import ccall safe "interface.h"
	set :: Bitmap -> CUInt -> CUInt -> CUInt -> CDouble -> IO()

foreign import ccall safe "interface.h"
	height :: Bitmap -> CUInt

foreign import ccall safe "interface.h"
	width :: Bitmap -> CUInt

foreign import ccall safe "interface.h"
	ul_y :: Bitmap -> CUInt

foreign import ccall safe "interface.h"
	ul_x :: Bitmap -> CUInt

foreign import ccall safe "interface.h"
	lr_y :: Bitmap -> CUInt

foreign import ccall safe "interface.h"
	lr_x :: Bitmap -> CUInt

foreign import ccall safe "interface.h"
	aoi_height :: Bitmap -> CUInt

foreign import ccall safe "interface.h"
	aoi_width :: Bitmap -> CUInt

