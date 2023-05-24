#pragma once

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <stdarg.h>
#include <png.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

struct Png {
    int width, height;
    png_byte color_type;
    png_byte bit_depth;
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytep *row_pointers;
};

typedef struct png_color_struct_count {
    png_byte red;
    png_byte green;
    png_byte blue;
    int amount;
} png_colorc;