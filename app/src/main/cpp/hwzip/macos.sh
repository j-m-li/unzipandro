clang generate_tables.c -o generate_tables && ./generate_tables > tables.c
clang -o hwzip  crc32.c deflate.c folder.c huffman.c hwzip.c implode.c lz77.c reduce.c shrink.c tables.c zip.c 

