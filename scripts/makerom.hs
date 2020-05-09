import Foreign.Marshal.Array (newArray)
import Control.Exception (bracket)
import Data.Int (Int8)
import System.IO (IOMode(..), openBinaryFile, hClose, hPutBuf, hFlush)

main :: IO ()
main = bracket (openBinaryFile "rom.bin" WriteMode) hClose $ \handle -> do
  let bytes = replicate 32768 (0xea :: Int8)
  p <- newArray bytes
  hPutBuf handle p (length bytes)
  hFlush handle
