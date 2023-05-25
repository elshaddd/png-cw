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

void black_white(struct Png *image, int x0, int y0, int x1, int y1);

void vert_concat(struct Png *png1, struct Png *png2, struct Png *result, png_color color);

void add_borders(struct Png *image, int border_size, png_color color);

void del_borders(struct Png *image, int border_size);

void square_with_point(struct Png *image, png_color color);

void merge(struct Png *png1, struct Png *png2, struct Png *result);

void paving(struct Png *img1, struct Png *img2);

void draw_line(struct Png *img, int x_s, int y_s, int x_e, int y_e, int width, png_color color);

void draw_circle(struct Png *img, int x_s, int y_s, int radius, int width, png_color color);

void skirting(struct Png *image, png_color color);

void reflection(struct Png* image,int mode, int x, int y, int size);