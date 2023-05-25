#include "read_and_write.h"
#include "functions.h"

static int f_draw;
static int f_swap;
static int f_change;
static int f_inversion;
static int f_function;

int main(int argc, char *argv[]) {
    struct Png image;
    int r = 0, g = 0, b = 0;

    png_color fill_color = {0};
    png_color outline = {0};
    int start[2] = {0};
    int size = 0;
    int line = 0;
    int is_fill = 0;

    int swap[4] = {0};
    int mode = 0;

    png_color color = {0};

    int inversion_coor[4] = {0};
    png_color color1 = {0};

    static struct option long_options[] = {
            {"draw",      no_argument, &f_draw,    1},
            {"swap",      required_argument, NULL, 'w'},
            {"mode",      required_argument, NULL, 'm'},
            {"change",    required_argument, NULL, 'c'},
            {"inversion", required_argument, NULL, 'i'},
            {"size",      required_argument, NULL, 'd'}, //side's size
            {"start",     required_argument, NULL, 's'},
            {"line",      required_argument, NULL, 'l'},
            {"outline",   required_argument, NULL, 'o'}, //line_color
            {"fill",      required_argument, NULL, 'f'},
            {"help",      no_argument,       NULL, 'h'},
            {"func",      no_argument,       NULL, 'z'},
            {"color",     required_argument, NULL, 'v'},
            {NULL, 0,                        NULL, 0}
    };

    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "w:m:c:i:s:d:l:o:f:h?", long_options, &option_index)) != -1) {
        switch (opt) {
            case 0:
                if (long_options[option_index].flag != 0) {
                    break;
                }

            case 'w':
                sscanf(optarg, "%d,%d:%d,%d", &swap[0], &swap[1], &swap[2], &swap[3]);
                f_swap += 1;
                break;
            case 'm':
                mode = atoi(optarg);
                break;

            case 'i':
                f_inversion += 1;
                sscanf(optarg, "%d,%d:%d,%d", &inversion_coor[0], &inversion_coor[1], &inversion_coor[2],
                       &inversion_coor[3]);
                break;

            case 'c':
                f_change += 1;
                sscanf(optarg, "%d,%d,%d", &r, &g, &b);
                color.red = r;
                color.green = g;
                color.blue = b;
                break;

            case 's':
                sscanf(optarg, "%d,%d", &start[0], &start[1]);
                break;

            case 'd':
                size = atoi(optarg);
                break;

            case 'l':
                line = atoi(optarg);
                break;

            case 'o':
                sscanf(optarg, "%d,%d,%d", &r, &g, &b);
                outline.red = r;
                outline.green = g;
                outline.blue = b;
                break;

            case 'f':
                sscanf(optarg, "%d,%d,%d", &r, &g, &b);
                fill_color.red = r;
                fill_color.green = g;
                fill_color.blue = b;
                is_fill = 1;
                break;

            case 'z': {
                f_function += 1;
                break;
            }
            case 'v':
                sscanf(optarg, "%d,%d,%d", &r, &g, &b);
                color1.red = r;
                color1.green = g;
                color1.blue = b;
                break;

            case 'h':
            case '?':
                printHelp();
                return 0;

            default:
                printf("Unknown option %c\n", opt);
                exit(EXIT_FAILURE);
        }
    }

    read_png_file(argv[optind++], &image);
    if (f_draw) {
        draw_square(&image, start[0], start[1], size, line, outline, is_fill, fill_color);
    }
    if (f_swap) {
        swap_pix(&image, mode, swap[0], swap[1], swap[2], swap[3]);
    }
    if (f_change) {
        change_color(&image, color);
    }
    if (f_inversion) {
        inversion(&image, inversion_coor[0], inversion_coor[1], inversion_coor[2], inversion_coor[3]);
    }
    if (f_function) {
//        ADD FUNC HERE
//        for example
//        crop(&copy,0,0,40,40);
//        read_png_file("kub.png",&image1);

//        write_png_file("resultat.png",&result);
    }
    write_png_file(argv[optind++], &image);

    if (optind < argc) {
        printf("non-option: ");
        while (optind < argc) {
            printf("%s ", argv[optind++]);
        }
        printf("\n");
    }

    exit(EXIT_SUCCESS);
}