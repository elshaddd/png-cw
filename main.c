#include "read_and_write.h"
#include "functions.h"

//int main(int argc, char **argv) {
//    struct Png image;
//    read_png_file(argv[1], &image);
//    int x0 = 0;
//    int y0 = 0;
//    int x1 = 200;
//    int y1 = 200;
//    int x = 10;
//    int y = 10;
//    int size = 30;
//    int line_width = 4;
//    png_color line_color = {255, 255, 255};
//    int is_filled = 1;
//    png_color fill_color = {1, 2, 3};
//    int mode = 1;
////    png_color sw_color = {60, 179, 113};
////    draw_square(x, y, size, line_width, line_color, is_filled,fill_color, &image);
////    negative(x0,y0,x1,y1,&image);
//    swap_pix(&image, mode);
////    change_color(&image, sw_color);
//
//
//    write_png_file(argv[2], &image);
//    return 0;
//}

void printHelp() {
    printf("possibly options\n");
    printf("--final <value> - final value\n");
    printf("--reverse - reverse\n");
    printf("-v --verbose - verbose\n");
    printf("-h -? - help\n");
}
static int f_draw;
static int f_change;
static int f_inv;
static int f_blocks;


int main(int argc, char *argv[]) {
    struct Png image;
    int r =0 ,g = 0,b = 0;
    png_color color = {0};

    png_color pouring = {0};
    png_color outline = {0};
//    int start[2]={0};
//    int end[2]={0};
    int sq_coor[2] = {0};
    int side = 0;
    int line = 0;
    int fill = 0;

    int blocks[4] = {0};
    int mode=0;

    int inversion_coor[4]={0};

    static struct option long_options[] = {
            {"draw", no_argument, &f_draw, 1},
            {"blocks", required_argument, NULL, 'b'},
            {"mode", required_argument, NULL, 'm'},
            {"change", required_argument, NULL , 'c'},
            {"inversion", required_argument, NULL, 'i'},
//            {"color", required_argument, NULL, 'c'},
            {"side", required_argument, NULL, 'd'}, //dimension
            {"coor", required_argument, NULL, 'r'},
            {"line", required_argument, NULL, 'l'},
            {"outline", required_argument, NULL, 'o'}, //outline
            {"pouring", required_argument, NULL, 'p'}, //pouring
            {"fill", required_argument, NULL, 'f' },
//            {"end", required_argument, NULL, 'e'},
            {"help", no_argument, NULL, 'h'},
            {NULL,      0,                 NULL, 0}
    };

    int opt;
    int option_index = 0;

    while ((opt=getopt_long(argc, argv, "b:m:c:i:d:r:l:o:p:f:h?", long_options, &option_index))!= -1) {
        switch (opt) {
            case 0:
                if (long_options[option_index].flag != 0) {
                    break;
                }
            case 'b':
                sscanf(optarg, "(%d,%d)(%d,%d)", &blocks[0], &blocks[1], &blocks[2], &blocks[3]);
                f_blocks+=1;
                break;
            case 'm':
                mode = atoi(optarg);
                break;
                // --blocks (10,20)(30,40) --mode 2

            case 'i':
                f_inv+=1;
                sscanf(optarg, "(%d,%d)(%d,%d)", &inversion_coor[0], &inversion_coor[1], &inversion_coor[2], &inversion_coor[3]);
                break;
                // --inversion (20,30)(300,200)

            case 'c':
                f_change+=1;
                sscanf(optarg, "(%d,%d,%d)", &r, &g, &b);
                color.red = r;
                color.green = g;
                color.blue = b;
                break;
                // --change (255,0,0)
                //--change (72,209,204) test5.png test5_res1.png
            case 'r':
                sscanf(optarg, "(%d,%d)", &sq_coor[0], &sq_coor[1]);
                break;

            case 'd':
                side = atoi(optarg);
                break;

            case 'l':
                line = atoi(optarg);
                break;

            case 'o':
                sscanf(optarg, "(%d,%d,%d)", &r, &g, &b);
                outline.red = r;
                outline.green = g;
                outline.blue = b;
                break;

            case 'p':
                sscanf(optarg, "(%d,%d,%d)", &r, &g, &b);
                pouring.red = r;
                pouring.green = g;
                pouring.blue = b;
                break;

            case 'f':
                fill = atoi(optarg);
                break;

                // --draw --change (72,209,204) --coor (0,0) --side 75 --line 5 --outline (0,255,0) --fill 1 --pouring (0,0,255) test5.png test5_res3.png
//            case 'e':
//                sscanf(optarg, "%d,%d", &end[0], &end[1]);
//                break;

            case 'h':
            case '?':
                printHelp();
                return 0;

            default:
                printf("Unknown option %c\n", opt);
                exit(EXIT_FAILURE);
        }
    }
//    printf("(%d %d %d %d) %d\n",inversion_coor[0],inversion_coor[1],inversion_coor[2],inversion_coor[3],f_inv);
    if (read_png_file(argv[optind++], &image)){
        printf("reading error\n");
    }else{
        if(f_draw){
            draw_square(&image, sq_coor[0], sq_coor[1], side, line, outline, fill, pouring);
        }
        if(f_blocks){
            swap_pix(&image,mode,blocks[0],blocks[1],blocks[2],blocks[3]);
        }
        if(f_change){
            change_color(&image,color);
        }
        if(f_inv){
            negative(&image, inversion_coor[0],inversion_coor[1],inversion_coor[2],inversion_coor[3]);
        }

    }
//    printf("blocks: %d\n", blocks);
//    if(f_draw){
//        draw_square(start[0], start[1], side, line, outline, fill, pouring, &image);
//    }
//    if(blocks){
//        swap_pix(&image,blocks,start[0],start[1],end[0],end[1]);
//    }
//    if(f_change){
//        change_color(&image,color);
//    }
//    if(f_inv){
//        negative(start[0],start[1],end[0],end[1],&image);
//    }
//    printf("blocks: %d\n", blocks);
    if (write_png_file(argv[optind++], &image)){
        printf("writing error\n");
    }


    if (optind < argc) {
        printf ("non-option ARGV-elements: ");
        while (optind < argc) {
            printf ("%s ", argv[optind++]);
        }
        printf ("\n");
    }
//    printf("start: %d %d\n", start[0], start[1]);
//    printf("end: %d %d\n", end[0], end[1]);
    exit(EXIT_SUCCESS);
}

//--inversion --draw --change --blocks 1 --color 20,4,7 --outline 255,0,70 --pouring 146,156,134 --end 20,20 --side 50
//--draw --start 10,10 --side 40 --line 4 --outline 255,255,255 --fill 1 --pouring 1,2,3 test.png test_res3.png
//--draw --line 4 --pouring 1,2,3 --start 10,10 --fill 1 --outline 255,255,255 --side 30
// --draw --side 30 --pouring 1,2,3 --outline 255,255,255 --line 4 --start 10,10 --fill 1
// --draw --start 10,10 --side 40 --line 4 --outline 255,255,255 --fill 1 --pouring 1,2,3
// --draw --line 4 --pouring 1,2,3 --start 10,10 --fill 1 --outline 255,255,255 --side 30 test6.png test6_res2.png
// --blocks 1 --start 0,0 --end 40,40 test2.png test2_res.png

//    printf("draw: %d\n", f_draw);
//    printf("blocks: %d\n", blocks);
//    printf("change: %d\n", f_change);
//    printf("inversion: %d\n", f_inv);
//    printf("color: %d %d %d\n",  color.red, color.green, color.blue);
//    printf("start: %d %d\n", start[0], start[1]);
//    printf("side: %d\n", side);
//    printf("line: %d\n", line);
//    printf("outline: %d %d %d\n", outline.red, outline.green, outline.blue);
//    printf("fill: %d\n", fill);
//    printf("pouring: %d %d %d\n", pouring.red, pouring.green, pouring.blue);
//    printf("end: %d %d\n", end[0], end[1]);