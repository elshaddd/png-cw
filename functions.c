#include "functions.h"

void printHelp() {
    printf("\nPossibly options:\n");
    printf("--draw - Drawing a square. A square is defined:\n");
    printf("--start (<start coordinates>) - The coordinates of the upper left corner;\n");
    printf("--size <value> - The size of the side;\n");
    printf("--line <value> - The thickness of the lines;\n");
    printf("--outline (<color values>) - The color of the lines;\n");
    printf("--fill (<color values>) - Filled color (optional).\n");
    printf("--swap (<start coordinates>)(<end coordinates>) - Swap four pieces of the target area. Swap is defined:\n");
    printf("--mode <value> - The method of swaping pieces: \"in a circle\" - <1>, \"diagonally\" - <2>.\n");
    printf("--change (<color values>) - The most common color is replaced by another.\n");
    printf("--inversion (<start coordinates>)(<end coordinates>) - Color inversion in the target area.\n");
    printf("\nThe last two options are the names of PNG files for reading and writing.\n\n");
    printf("Examples of use:\n"
           "--draw --start (0,0) --size 100 --line 4 --outline (255,0,0) --fill (0,255,0) test.png test_res.png\n"
           "--swap (10,20)(100,200) --mode 2 test.png test_res.png\n"
           "--change (0,128,128) test.png test_res.png\n"
           "--inversion (0,0)(700,700) test.png test_res.png\n");
}


void draw_square(struct Png *image, int x0, int y0, int size, int line_width, png_color line_color, int is_fill,
                 png_color fill_color) {
    if (((size - x0) > image->width) || ((size - y0) > image->height)) {
        printf("the coordinates do not match the size of the image\n");
        return;
    }

    if ((line_width * 2 > image->height) || (line_width * 2 > image->width)) {
        printf("line width is too big");
        return;
    }

    if (png_get_color_type(image->png_ptr, image->info_ptr) == PNG_COLOR_TYPE_RGB) {
        printf("input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA");
        return;// Some error handling: input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA
    }

    if (png_get_color_type(image->png_ptr, image->info_ptr) != PNG_COLOR_TYPE_RGBA) {
        printf("input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA");
        return;// Some error handling: color_type of input file must be PNG_COLOR_TYPE_RGBA
    }

// Calculate coordinates of bottom right corner of square
    int x1 = x0 + size - 1;
    int y1 = y0 + size - 1;

// Draw left and right line of square
    for (int i = y0; i <= y1; i++) {
        png_bytep row = image->row_pointers[i];
        for (int j = 0; j < line_width; j++) {
            png_bytep ptr_left = &(row[(x0 + j) * 4]);
            png_bytep ptr_right = &(row[(x1 - j) * 4]);

            ptr_left[0] = line_color.red;
            ptr_left[1] = line_color.green;
            ptr_left[2] = line_color.blue;

            ptr_right[0] = line_color.red;
            ptr_right[1] = line_color.green;
            ptr_right[2] = line_color.blue;
        }
    }
// Draw top and bottom line of square
    for (int i = 0; i < line_width; i++) {
        png_bytep row_top = image->row_pointers[y0 + i];
        png_bytep row_bot = image->row_pointers[y1 - i];
        for (int j = x0; j <= x1; j++) {
            png_bytep ptr_top = &(row_top[j * 4]);
            png_bytep ptr_bot = &(row_bot[j * 4]);

            ptr_top[0] = line_color.red;
            ptr_top[1] = line_color.green;
            ptr_top[2] = line_color.blue;

            ptr_bot[0] = line_color.red;
            ptr_bot[1] = line_color.green;
            ptr_bot[2] = line_color.blue;
        }
    }
// Fill square if requested
    if (is_fill) {
        for (int i = y0 + line_width; i <= y1 - line_width; i++) {
            png_bytep row = image->row_pointers[i];
            for (int j = x0 + line_width; j <= x1 - line_width; j++) {
                png_bytep ptr = &(row[j * 4]);
                ptr[0] = fill_color.red;
                ptr[1] = fill_color.green;
                ptr[2] = fill_color.blue;
            }
        }
    }
}


void swapArrays(png_byte *arr1, png_byte *arr2) {
    for (int i = 0; i < 3; i++) {
        int temp = *(arr1 + i);
        *(arr1 + i) = *(arr2 + i);
        *(arr2 + i) = temp;
    }
}


void swap_pix(struct Png *image, int mode, int x0, int y0, int x1, int y1) {
    if (((x1 - x0) > image->width) || ((y1 - y0) > image->height)) {
        printf("the coordinates do not match the size of the image\n");
        return;
    }

    if (png_get_color_type(image->png_ptr, image->info_ptr) == PNG_COLOR_TYPE_RGB) {
        printf("input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA");
        return;// Some error handling: input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA
    }

    if (png_get_color_type(image->png_ptr, image->info_ptr) != PNG_COLOR_TYPE_RGBA) {
        printf("color_type of input file must be PNG_COLOR_TYPE_RGBA");
        return;// Some error handling: color_type of input file must be PNG_COLOR_TYPE_RGBA
    }
    // Median
    int h_med = (x1 - x0) / 2;
    int v_med = (y1 - y0) / 2;

    for (int i = y0; i < v_med; i++) {
        png_bytep row1 = image->row_pointers[i];
        png_bytep row2 = image->row_pointers[v_med + i];
        for (int j = x0; j < h_med; j++) {
            // Partitioning the four parts of the image into pointers
            png_bytep ptr_1 = &(row1[j * 4]);
            png_bytep ptr_2 = &(row1[(h_med + j) * 4]);
            png_bytep ptr_3 = &(row2[(h_med + j) * 4]);
            png_bytep ptr_4 = &(row2[j * 4]);
            if (mode == 1) {
                // Round swapping
                swapArrays(ptr_1, ptr_2);
                swapArrays(ptr_1, ptr_3);
                swapArrays(ptr_1, ptr_4);
            } else if (mode == 2) {
                // Diagonal swapping
                swapArrays(ptr_1, ptr_3);
                swapArrays(ptr_2, ptr_4);
            } else {
                printf("no such mode");
            }

        }
    }
}


void change_color(struct Png *image, png_color color) {
    int arr_size = (image->width) * (image->height);
    // Array to store all pixel colors png_colorc
    png_colorc *arr = malloc(arr_size * sizeof(png_colorc));
    int c = 0;
    for (int i = 0; i < image->height; i++) {
        png_bytep row = image->row_pointers[i];
        for (int j = 0; j < image->width; j++) {
            int fl = 0;
            png_bytep ptr = &(row[j * 4]);
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
    png_colorc color_need_to_change;
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
            png_bytep ptr = &(row[j * 4]);
            if (ptr[0] == color_need_to_change.red && ptr[1] == color_need_to_change.green &&
                ptr[2] == color_need_to_change.blue) {
                ptr[0] = color.red;
                ptr[1] = color.green;
                ptr[2] = color.blue;
            }

        }
    }
}


void inversion(struct Png *image, int x0, int y0, int x1, int y1) {
    if (((x1 - x0) > image->width) || ((y1 - y0) > image->height)) {
        printf("the coordinates do not match the size of the image\n");
        return;
    }

    if (png_get_color_type(image->png_ptr, image->info_ptr) == PNG_COLOR_TYPE_RGB) {
        return;// Some error handling: input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA
    }

    if (png_get_color_type(image->png_ptr, image->info_ptr) != PNG_COLOR_TYPE_RGBA) {
        return;// Some error handling: color_type of input file must be PNG_COLOR_TYPE_RGBA
    }

    for (int i = y0; i < y1; i++) {
        png_bytep row = image->row_pointers[i];
        for (int j = x0; j < x1; j++) {
            png_bytep ptr = &(row[j * 4]);
            // Color inversion: 255 - <value>
            ptr[0] = 255 - ptr[0];
            ptr[1] = 255 - ptr[1];
            ptr[2] = 255 - ptr[2];
        }
    }
}


void black_white(struct Png *image, int x0, int y0, int x1, int y1) {
    if (((x1 - x0) > image->width) || ((y1 - y0) > image->height)) {
        printf("the coordinates do not match the size of the image\n");
        return;
    }

    if (png_get_color_type(image->png_ptr, image->info_ptr) == PNG_COLOR_TYPE_RGB) {
        return;// Some error handling: input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA
    }

    if (png_get_color_type(image->png_ptr, image->info_ptr) != PNG_COLOR_TYPE_RGBA) {
        return;// Some error handling: color_type of input file must be PNG_COLOR_TYPE_RGBA
    }

    for (int i = y0; i < y1; i++) {
        png_bytep row = image->row_pointers[i];
        for (int j = x0; j < x1; j++) {
            png_bytep ptr = &(row[j * 4]);
            int gray = (ptr[0] + ptr[1] + ptr[2]) / 3;
            ptr[0] = gray;
            ptr[1] = gray;
            ptr[2] = gray;
        }
    }
}
