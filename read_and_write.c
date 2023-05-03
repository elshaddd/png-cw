#include "read_and_write.h"


int read_png_file(char *file_name, struct Png *image) {

    char header[8];    // 8 is the maximum size that can be checked

    /* open file and test for it being a png */
    FILE *fp = fopen(file_name, "rb");
    if (!fp) {
        printf("[read_png_file] File %s could not be opened for reading\n", file_name);
        return 1;
    }

    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8)) {
        printf("[read_png_file] File %s is not recognized as a PNG file\n", file_name);
        return 1;
    }

    /* initialize stuff */
    image->png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!image->png_ptr) {
        printf("[read_png_file] png_create_read_struct failed\n");
        return 1;
    }

    image->info_ptr = png_create_info_struct(image->png_ptr);
    if (!image->info_ptr) {
        printf("[read_png_file] png_create_info_struct failed\n");
        return 1;
    }

    if (setjmp(png_jmpbuf(image->png_ptr))) {
        printf("[read_png_file] Error during init_io\n");
        png_destroy_read_struct(&image->png_ptr, &image->info_ptr, NULL);
        return 1;
    }

    png_init_io(image->png_ptr, fp);
    png_set_sig_bytes(image->png_ptr, 8);

    png_read_info(image->png_ptr, image->info_ptr);

    image->width = png_get_image_width(image->png_ptr, image->info_ptr);
    image->height = png_get_image_height(image->png_ptr, image->info_ptr);
    image->color_type = png_get_color_type(image->png_ptr, image->info_ptr);
    image->bit_depth = png_get_bit_depth(image->png_ptr, image->info_ptr);

//    if (image->color_type == PNG_COLOR_TYPE_PALETTE)
//        png_set_palette_to_rgb(image->png_ptr);

    if (image->color_type == PNG_COLOR_TYPE_RGB || image->color_type == PNG_COLOR_TYPE_RGB_ALPHA)
        png_set_add_alpha(image->png_ptr, 0xFF, PNG_FILLER_AFTER);

    if (image->color_type == PNG_COLOR_TYPE_GRAY && image->bit_depth < 8)
        png_set_expand_gray_1_2_4_to_8(image->png_ptr);

    if (png_get_valid(image->png_ptr, image->info_ptr, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(image->png_ptr);
    } else {
        png_set_add_alpha(image->png_ptr, 0xff, PNG_FILLER_AFTER);
    }

    if (image->bit_depth == 16)
        png_set_strip_16(image->png_ptr);

    if (image->color_type == PNG_COLOR_TYPE_GRAY || image->color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(image->png_ptr);

    png_read_update_info(image->png_ptr, image->info_ptr);

    /* read file */
    if (setjmp(png_jmpbuf(image->png_ptr))) {
        printf("[read_png_file] Error during read_image\n");
        return 1;
    }

    image->row_pointers = (png_bytep *) malloc(sizeof(png_bytep) * image->height);
    for (int y = 0; y < image->height; y++)
        image->row_pointers[y] = (png_byte *) malloc(png_get_rowbytes(image->png_ptr, image->info_ptr));

    png_read_image(image->png_ptr, image->row_pointers);

    fclose(fp);

}


int write_png_file(char *file_name, struct Png *image) {
    FILE *fp = fopen(file_name, "wb");
    if (!fp) {
        printf("[write_png_file] File %s could not be opened for writing\n", file_name);
        return 1;
    }

    /* initialize stuff */
    image->png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!image->png_ptr) {
        printf("[write_png_file] png_create_write_struct failed\n");
        return 1;
    }

    image->info_ptr = png_create_info_struct(image->png_ptr);
    if (!image->info_ptr) {
        printf("[write_png_file] png_create_info_struct failed\n");
        return 1;
    }

    if (setjmp(png_jmpbuf(image->png_ptr))) {
        printf("[write_png_file] Error during init_io\n");
        return 1;
    }

    png_init_io(image->png_ptr, fp);

    /* write header */
    if (setjmp(png_jmpbuf(image->png_ptr))) {
        printf("[write_png_file] Error during writing header\n");
        return 1;
    }

    png_set_IHDR(image->png_ptr, image->info_ptr, image->width, image->height,
                 image->bit_depth, image->color_type, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(image->png_ptr, image->info_ptr);

    /* write bytes */
    if (setjmp(png_jmpbuf(image->png_ptr))) {
        printf("[write_png_file] Error during writing bytes\n");
        return 1;
    }

    png_write_image(image->png_ptr, image->row_pointers);

    /* end write */
    if (setjmp(png_jmpbuf(image->png_ptr))) {
        printf("[write_png_file] Error during end of write\n");
        return 1;
    }

    png_write_end(image->png_ptr, NULL);    //png_write_end(image->png_ptr, image->info_ptr)

    /* cleanup heap allocation */
    png_destroy_write_struct(&image->png_ptr, &image->info_ptr);
    fclose(fp);

    /* clean up */
    png_destroy_read_struct(&image->png_ptr, &image->info_ptr, NULL);
    for (int y = 0; y < image->height; y++)
        free(image->row_pointers[y]);
    free(image->row_pointers);

}