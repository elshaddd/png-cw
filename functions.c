#include "functions.h"


void printHelp() {
    printf("\nPossibly options:\n");
    printf("\nDrawing a square.\n");
    printf("--draw - Command to draw. A square is defined:\n");
    printf("--start <start coordinates> - The coordinates of the upper left corner;\n");
    printf("--size <value> - The size of the side;\n");
    printf("--line <value> - The thickness of the lines;\n");
    printf("--outline <color values> - The color of the lines;\n");
    printf("--fill <color values> - Filled color (optional).\n");
    printf("\nSwap four pieces of the target area.\n");
    printf("--swap <start coordinates>:<end coordinates> - Command to swap. Swap is defined:\n");
    printf("--mode <value> - The method of swaping pieces: \"in a circle\" - <1>, \"diagonally\" - <2>.\n");
    printf("\nThe most common color is replaced by another.\n");
    printf("--change <color values> - Command to change color.\n");
    printf("\nColor inversion in the target area.\n");
    printf("--inversion <start coordinates>:<end coordinates> - Command to inverse.\n");
    printf("\nThe last two options are the names of PNG files for reading and writing.\n");
    printf("Please be careful with the spaces and observe the option formats.\n\n");
    printf("Examples of use:\n"
           "--draw --start 0,0 --size 100 --line 4 --outline 255,0,0 --fill 0,255,0 test.png test_res.png\n"
           "--swap 10,20:100,200 --mode 2 test.png test_res.png\n"
           "--change 0,128,128 test.png test_res.png\n"
           "--inversion 0,0:700,700 test.png test_res.png\n\n");
}


void draw_square(struct Png *image, int x0, int y0, int size, int line_width, png_color line_color, int is_fill,
                 png_color fill_color) {
    if (((size - x0) > image->width) || ((size - y0) > image->height)) {
        printf("the size of square do not match the size of the image\n");
        return;
    }

    if ((line_width * 2 > image->height) || (line_width * 2 > image->width)) {
        printf("line width is too big");
        return;
    }
// Calculate coordinates of bottom right corner of square
    int x1 = x0 + size - 1;
    int y1 = y0 + size - 1;
// Draw left and right line of square
    for (int i = y0; i <= y1; i++) {
        for (int j = 0; j < line_width; j++) {
            set_pixel_color(image, x0 + j, i, line_color);
            set_pixel_color(image, x1 - j, i, line_color);
        }
    }
// Draw top and bottom line of square
    for (int i = 0; i < line_width; i++) {
        for (int j = x0; j <= x1; j++) {
            set_pixel_color(image, j, y0 + i, line_color);
            set_pixel_color(image, j, y1 - i, line_color);
        }
    }
// Fill square if requested
    if (is_fill) {
        for (int i = y0 + line_width; i <= y1 - line_width; i++) {
            for (int j = x0 + line_width; j <= x1 - line_width; j++) {
                set_pixel_color(image, j, i, fill_color);
            }
        }
    }
}


void swap_pix(struct Png *image, int mode, int x0, int y0, int x1, int y1) {
    if (((x1 - x0) >= image->width) || ((y1 - y0) >= image->height) || (x1 >= image->width) || (y1 >= image->height)) {
        printf("the coordinates do not match the size of the image\n");
        return;
    }
    if ((mode != 1 && mode != 2)) {
        printf("no such mode\n");
        return;
    }
    // Oddness of the sides of the area
    if ((x1 - x0 + 1) % 2 != 0 || (y1 - y0 + 1) % 2 != 0) {
        printf("some parts of the image were not used for the change\n");
    }
    // Median
    int h_med = (x1 - x0) / 2;
    int v_med = (y1 - y0) / 2;

    for (int i = y0; i < v_med; i++) {
        png_bytep row1 = image->row_pointers[i];
        png_bytep row2 = image->row_pointers[v_med + i - y0];
        for (int j = x0; j < h_med; j++) {
            // Partitioning the four parts of the image into pointers
            png_bytep ptr_1 = &(row1[j * image->channels]);
            png_bytep ptr_2 = &(row1[(h_med + j - x0) * image->channels]);
            png_bytep ptr_3 = &(row2[(h_med + j - x0) * image->channels]);
            png_bytep ptr_4 = &(row2[j * image->channels]);
            if (mode == 1) {
                // Round swapping
                swap_arrays(ptr_1, ptr_2, image->channels);
                swap_arrays(ptr_1, ptr_3, image->channels);
                swap_arrays(ptr_1, ptr_4, image->channels);
            } else {
                // Diagonal swapping
                swap_arrays(ptr_1, ptr_3, image->channels);
                swap_arrays(ptr_2, ptr_4, image->channels);
            }
        }
    }
}


void change_color(struct Png *image, png_color color) {
    int arr_size = (image->width) * (image->height);
    // Array to store all pixel colors png_colora
    png_colora *arr = malloc(arr_size * sizeof(png_colora));
    int c = 0;
    for (int i = 0; i < image->height; i++) {
        png_bytep row = image->row_pointers[i];
        for (int j = 0; j < image->width; j++) {
            int fl = 0;
            png_bytep ptr = &(row[j * image->channels]);
            // Only new colors are added to the array, and each color's counter is incremented when repeating
            for (int k = 0; k < c; k++) {
                if ((ptr[0] == arr[k].red) && (ptr[1] == arr[k].green) && (ptr[2] == arr[k].blue)) {
                    arr[k].amount += 1;
                    fl = 1;
                    break;
                }
            }
            if (!fl) {
                arr[c].red = ptr[0];
                arr[c].green = ptr[1];
                arr[c].blue = ptr[2];
                c += 1;
            }
        }
    }
// Searching for a color in the array with the largest counter
    int max = 0;
    png_colora color_need_to_change;
    for (int i = 0; i < c; i++) {
        if (arr[i].amount > max) {
            max = arr[i].amount;
            color_need_to_change.red = arr[i].red;
            color_need_to_change.green = arr[i].green;
            color_need_to_change.blue = arr[i].blue;
        }
    }
// Color change
    for (int i = 0; i < image->height; i++) {
        png_bytep row = image->row_pointers[i];
        for (int j = 0; j < image->width; j++) {
            png_bytep ptr = &(row[j * image->channels]);
            if (ptr[0] == color_need_to_change.red && ptr[1] == color_need_to_change.green &&
                ptr[2] == color_need_to_change.blue) {
                ptr[0] = color.red;
                ptr[1] = color.green;
                ptr[2] = color.blue;
            }
        }
    }
    free(arr);
}


void inversion(struct Png *image, int x0, int y0, int x1, int y1) {
    if (((x1 - x0) >= image->width) || ((y1 - y0) >= image->height)) {
        printf("the coordinates do not match the size of the image\n");
        return;
    }

    for (int i = y0; i <= y1; i++) {
        png_bytep row = image->row_pointers[i];
        for (int j = x0; j <= x1; j++) {
            png_bytep ptr = &(row[j * image->channels]);
            // Color inversion: 255 - <value>
            ptr[0] = 255 - ptr[0];
            ptr[1] = 255 - ptr[1];
            ptr[2] = 255 - ptr[2];
        }
    }
}


void swap_arrays(png_byte *arr1, png_byte *arr2, png_byte amount) {
    for (int i = 0; i < amount; i++) {
        int temp = *(arr1 + i);
        *(arr1 + i) = *(arr2 + i);
        *(arr2 + i) = temp;
    }
}


bool valid_coor(int x, int y, int height, int width) {
    if (y < height && x < width && x >= 0 && y >= 0) {
        return true;
    }
    return false;
}


void set_pixel_color(struct Png *img, int x, int y, png_color color) {
    if (valid_coor(x, y, img->height, img->width) == false)
        return;
    int f_rgb = img->channels;
    png_bytep ptr = &(img->row_pointers[y][x * img->channels]);
    ptr[0] = color.red;
    ptr[1] = color.green;
    ptr[2] = color.blue;
    if (f_rgb == 4) {
        ptr[3] = 255;
    }
}


void set_pixel(struct Png *des, struct Png *src, int src_x, int src_y, int des_x, int des_y) {
    if (valid_coor(src_x, src_y, des->height, des->width) == false)
        return;
    if (valid_coor(des_x, des_y, src->height, src->width) == false)
        return;
    int f_rgb = src->channels;

    for (int k = 0; k < f_rgb; k++) {
        des->row_pointers[des_y][des_x * des->channels + k] = src->row_pointers[src_y][src_x * src->channels + k];
    }
}


void canvas(struct Png *image, png_byte color_type, png_byte bit_depth, int height, int width, png_color color) {
    image->width = width;
    image->height = height;
    image->color_type = color_type;
    image->bit_depth = bit_depth;
    image->row_pointers = (png_bytep *) malloc(sizeof(png_bytep) * height);
    for (int i = 0; i < height; i++) {
        image->row_pointers[i] = (png_byte *) malloc(sizeof(png_byte) * width * image->channels);
    }
    // set all pix to white
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            set_pixel_color(image, j, i, color);
        }
    }
}

void crop(struct Png *img, int x0, int y0, int x1, int y1) {
    struct Png copy;
    memcpy(&copy, img, sizeof(struct Png));
    png_color black = {0};
    canvas(img, img->color_type, img->bit_depth, (y1 - y0) + 1, (x1 - x0) + 1, black);
    for (int i = y0; i <= y1; i++) {
        for (int j = x0; j <= x1; j++) {
            set_pixel(img, &copy, j - x0, i - y0, j, i);
        }
    }
}

// paste from src to des in x,y
void paste(struct Png *des, struct Png *src, int x, int y) {
    for (int i = y; i <= y + src->height; i++) {
        for (int j = x; j <= x + src->width; j++) {
            set_pixel(des, src, j, i, j - x, i - y);
        }
    }
}