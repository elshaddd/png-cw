#include "functions.h"
#include "read_and_write.h"

void printHelp() {
    printf("\nPossibly options:\n");
    printf("\nDrawing a square.\n");
    printf("--draw - Command to draw. A square is defined:\n");
    printf("--start (<start coordinates>) - The coordinates of the upper left corner;\n");
    printf("--size <value> - The size of the side;\n");
    printf("--line <value> - The thickness of the lines;\n");
    printf("--outline (<color values>) - The color of the lines;\n");
    printf("--fill (<color values>) - Filled color (optional).\n");
    printf("\nSwap four pieces of the target area.\n");
    printf("--swap (<start coordinates>)(<end coordinates>) - Command to swap. Swap is defined:\n");
    printf("--mode <value> - The method of swaping pieces: \"in a circle\" - <1>, \"diagonally\" - <2>.\n");
    printf("\nThe most common color is replaced by another.\n");
    printf("--change (<color values>) - Command to change color.\n");
    printf("\nColor inversion in the target area.\n");
    printf("--inversion (<start coordinates>)(<end coordinates>) - Command to inverse.\n");
    printf("\nThe last two options are the names of PNG files for reading and writing.\n");
    printf("Please be careful with the spaces and observe the option formats.\n\n");
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
    // Oddness of the sides of the area
    if ((x1 - x0) % 2 != 0 || (y1 - y0) != 0) {
        printf("some parts of the image were not used for the change\n");
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


void vert_concat(struct Png *png1, struct Png *png2, struct Png *result) {
    // find maxH maxW
    int max_width = (png1->width > png2->width) ? png1->width : png2->width;
    int total_height = png1->height + png2->height;
    // set
    result->width = max_width;
    result->height = total_height;
    result->color_type = png1->color_type;
    result->bit_depth = png1->bit_depth;

    result->row_pointers = (png_bytep *) malloc(sizeof(png_bytep) * total_height);
    for (int i = 0; i < total_height; i++) {
        result->row_pointers[i] = (png_byte *) malloc(sizeof(png_byte) * max_width * 4); // RGBA
    }
    // first
    for (int i = 0; i < png1->height; i++) {
        for (int j = 0; j < png1->width; j++) {
            for (int k = 0; k < 4; k++) {
                result->row_pointers[i][j * 4 + k] = png1->row_pointers[i][j * 4 + k];
            }
        }
        // if f<s
        for (int j = png1->width; j < max_width; j++) {
            result->row_pointers[i][j * 4] = 255; // R
            result->row_pointers[i][j * 4 + 1] = 255; // G
            result->row_pointers[i][j * 4 + 2] = 255; // B
            result->row_pointers[i][j * 4 + 3] = 255; // A
        }
    }
    // second
    for (int i = 0; i < png2->height; i++) {
        for (int j = 0; j < png2->width; j++) {
            for (int k = 0; k < 4; k++) {
                result->row_pointers[i + png1->height][j * 4 + k] = png2->row_pointers[i][j * 4 + k];
            }
        }
        // if f>s
        for (int j = png2->width; j < max_width; j++) {
            result->row_pointers[i + png1->height][j * 4] = 255; // R
            result->row_pointers[i + png1->height][j * 4 + 1] = 255; // G
            result->row_pointers[i + png1->height][j * 4 + 2] = 255; // B
            result->row_pointers[i + png1->height][j * 4 + 3] = 255; // A
        }
    }
}


void add_borders(struct Png *image, int border_size, png_color color) {
    int newW = image->width + 2 * border_size;
    int newH = image->height + 2 * border_size;

    png_bytep *new_row_ptrs = (png_bytep *) malloc(sizeof(png_bytep) * newH);
    for (int i = 0; i < newH; i++) {
        new_row_ptrs[i] = (png_byte *) malloc(png_get_rowbytes(image->png_ptr, image->info_ptr) + 2 * border_size);
//        new_row_ptrs[i] = (png_byte *)malloc(10000);
        if (i < border_size || i >= border_size + image->height) {
            // up&down
            for (int j = 0; j < newW; j++) {
                png_bytep ptr = &(new_row_ptrs[i][j * 4]);
                ptr[0] = color.red;   // set color
                ptr[1] = color.green;
                ptr[2] = color.blue;
                ptr[3] = 0; // if u need invisible border a = 0
            }
        } else {
            png_bytep src_row = image->row_pointers[i - border_size];
            png_bytep trg_row = new_row_ptrs[i];
            for (int j = 0; j < newW; j++) {
                if (j < border_size || j >= border_size + image->width) {
                    // r-l borders
                    png_bytep ptr = &(trg_row[j * 4]);
                    ptr[0] = color.red;
                    ptr[1] = color.green;
                    ptr[2] = color.blue;
                    ptr[3] = 0;
                } else {
                    // Копируем пиксели из исходного изображения
                    png_bytep src_ptr = &(src_row[(j - border_size) * 4]);
                    png_bytep trg_ptr = &(trg_row[j * 4]);
                    trg_ptr[0] = src_ptr[0];
                    trg_ptr[1] = src_ptr[1];
                    trg_ptr[2] = src_ptr[2];
                    trg_ptr[3] = src_ptr[3];
                }
            }
        }
    }
    // cleanup
    for (int i = 0; i < image->height; i++) {
        free(image->row_pointers[i]);
    }
    free(image->row_pointers);
    // save new par
    image->width = newW;
    image->height = newH;
    image->row_pointers = new_row_ptrs;
}


void del_borders(struct Png *image, int border_size) {
    int newW = image->width - 2 * border_size;
    int newH = image->height - 2 * border_size;

    png_bytep *new_row_ptrs = (png_bytep *) malloc(sizeof(png_bytep) * newH);
    for (int i = 0; i < newH; i++) {
        new_row_ptrs[i] = (png_byte *) malloc(png_get_rowbytes(image->png_ptr, image->info_ptr));
        png_bytep src_row = image->row_pointers[i + border_size];
        png_bytep trg_row = new_row_ptrs[i];
        for (int j = 0; j < newW; j++) {
            png_bytep src_ptr = &(src_row[(j + border_size) * 4]);
            png_bytep trg_ptr = &(trg_row[j * 4]);
            trg_ptr[0] = src_ptr[0];
            trg_ptr[1] = src_ptr[1];
            trg_ptr[2] = src_ptr[2];
            trg_ptr[3] = src_ptr[3];
        }

    }
    // cleanup
    for (int i = 0; i < image->height; i++) {
        free(image->row_pointers[i]);
    }
    free(image->row_pointers);
    // save new par
    image->width = newW;
    image->height = newH;
    image->row_pointers = new_row_ptrs;
}


void square_with_point(struct Png *image, png_color color) {
    png_color fill_color = {0};
    add_borders(image, 1, fill_color);
    for (int i = 0; i < image->height; i++) {
        png_bytep row = image->row_pointers[i];
        for (int j = 0; j < image->width; j++) {
            png_bytep ptr = &(row[j * 4]);
            if (ptr[0] == 255 && ptr[1] == 0 && ptr[2] == 0) {
                draw_square(image, j - 1, i - 1, 3, 1, color, 0, fill_color);
            }
        }
    }
    del_borders(image, 1);
}


void merge(struct Png *png1, struct Png *png2, struct Png *result) {
    int max_width = (png1->width > png2->width) ? png1->width : png2->width;
    int max_height = (png1->height > png2->height) ? png1->height : png2->height;

    int total_height = max_height;
    int total_width = 2 * max_width;
    // set
    result->width = total_width;
    result->height = total_height;
    result->color_type = png1->color_type;
    result->bit_depth = png1->bit_depth;

    result->row_pointers = (png_bytep *) malloc(sizeof(png_bytep) * total_height);
    for (int i = 0; i < total_height; i++) {
        result->row_pointers[i] = (png_byte *) malloc(sizeof(png_byte) * total_width * 4);
    }
    // set all pix to white
    for (int i = 0; i < total_height; i++) {
        png_bytep row = result->row_pointers[i];
        for (int j = 0; j < total_width; j++) {
            png_bytep ptr = &(row[j * 4]);
            ptr[0] = 255;
            ptr[1] = 255;
            ptr[2] = 255;
            ptr[3] = 255;
        }
    }
    // even to png1
    for (int i = 0; i < png1->height; i++) {
        for (int j = 0; j < png1->width * 2; j++) {
            for (int k = 0; k < 4; k++) {
                if (j % 2 == 0) {
                    result->row_pointers[i][j * 4 + k] = png1->row_pointers[i][(j / 2) * 4 + k];
                }
            }
        }
    }
    // odds to png2
    for (int i = 0; i < png2->height; i++) {
        for (int j = 0; j < png2->width * 2; j++) {
            for (int k = 0; k < 4; k++) {
                if (j % 2 == 1) {
                    result->row_pointers[i][j * 4 + k] = png2->row_pointers[i][(j / 2) * 4 + k];
                }
                if (j % 2 == 0) {
                    continue;
                }
            }
        }
    }
}