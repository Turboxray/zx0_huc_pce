You'll need to obtain the executable for zx0 compressor.
Emmanuel Marty's SALVADOR ZX0 compressor which can be found here ...
https://github.com/emmanuel-marty/salvador

Windows builds can be found in the "Releases" link (on the right side of the main page)
The example batch files show how to call the compressor. Make sure to use the -classic
mode, otherwise it will not decompress correctly.

The -w option sets the "buffer" window size. For vram decompression, this will be the required
ring-buffer size. As of this example, I left the window ring-buffer size as 2048 bytes of ram.
If you change the size to something smaller, make sure not to create a compressed filed with an
expected larger ring-buffer size.

