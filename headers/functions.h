//#ifndef TEST_FUNCTIONS_H
//#define TEST_FUNCTIONS_H
//
//#endif //TEST_FUNCTIONS_H
#pragma once

#include "libs_and_strucs.h"

void draw_square(struct Png *image, int x0, int y0, int size, int line_width, png_color line_color, int is_filled, png_color fill_color);

void swapArrays(png_byte *arr1, png_byte *arr2);

void swap_pix(struct Png *image, int mode,int x0,int y0, int x1,int y1);

png_color switch_color(struct Png *image);

void negative(struct Png *image, int x0, int y0, int x1, int y1);

void change_color(struct Png *image, png_color color);

void che_be(struct Png *image, int x0, int y0, int x1, int y1);