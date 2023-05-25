#pragma once

#include "libs_and_strucs.h"

int read_png_file(char *file_name, struct Png *image);

int write_png_file(char *file_name, struct Png *image);