
{-# LANGUAGE ForeignFunctionInterface #-}

module Morphology where

import Interface
import Foreign
import Foreign.Ptr()
import Foreign.C.Types

foreign export ccall morphology :: Bitmap -> Bitmap

morphology :: Bitmap -> Bitmap

-- TODO timer_settime: Invalid argument
-- TODO алгоритм

morphology src = do
{
	create (height src) (width src)
}

