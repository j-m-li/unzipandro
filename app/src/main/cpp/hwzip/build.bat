cl /TC generate_tables.c && generate_tables > tables.c
cl /O2 /DNDEBUG /MT /Fehwzip.exe /TC crc32.c deflate.c folder.c huffman.c hwzip.c implode.c lz77.c reduce.c shrink.c tables.c zip.c /link setargv.obj

