//#ifndef TEST_READ_AND_WRITE_H
//#define TEST_READ_AND_WRITE_H
//
//#endif //TEST_READ_AND_WRITE_H
#pragma once

#include "libs_and_strucs.h"

int read_png_file(char *file_name, struct Png *image);

int write_png_file(char *file_name, struct Png *image);