#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <png.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

struct Png {
    int width, height;
    png_byte color_type;
    png_byte bit_depth;
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytep *row_pointers;
    png_byte channels;
};

typedef struct png_color_struct_amount {
    png_byte red;
    png_byte green;
    png_byte blue;
    int amount;
} png_colora;