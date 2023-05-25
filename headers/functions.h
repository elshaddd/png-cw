#pragma once

#include "libs_and_strucs.h"

void printHelp();

void swap_arrays(png_byte *arr1, png_byte *arr2, png_byte amount);

bool valid_coor(int x, int y, int height, int width);

void set_pixel_color(struct Png *img, int x, int y, png_color color);

void set_pixel(struct Png *des, struct Png *src, int src_x, int src_y, int des_x, int des_y);

void canvas(struct Png *image, png_byte color_type, png_byte bit_depth, int height, int width, png_color color);

void crop(struct Png *img, int x0, int y0, int x1, int y1);

void paste(struct Png *des, struct Png *src, int x, int y);

void draw_square(struct Png *image, int x0, int y0, int size, int line_width, png_color line_color, int is_fill,
                 png_color fill_color);

void swap_pix(struct Png *image, int mode, int x0, int y0, int x1, int y1);

void inversion(struct Png *image, int x0, int y0, int x1, int y1);

void change_color(struct Png *image, png_color color);