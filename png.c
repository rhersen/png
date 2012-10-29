#include <stdlib.h>
#include <sys/resource.h>
#include "png.h"

int main(int argc, char** argv) {
    void writeImage(FILE *file, png_bytepp data, int width, int height) { 
        png_structp p = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
        png_infop info = png_create_info_struct(p);
        setjmp(png_jmpbuf(p));
        png_init_io(p, file);
        png_set_rows(p, info, data);

        png_set_IHDR(p, info, width, height,
                     8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_ADAM7,
                     PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

        png_write_png(p, info, 0, png_voidp_NULL);
        png_destroy_write_struct(&p, &info);
    }

    void write_png(int width, int height) {
        png_bytepp getData(png_bytepp rows, png_bytep image) {
            for (int i = 0; i < height; ++i) {
                rows[i] = image + width * 3 * i;

                for (int j = 0; j < width; ++j) {
                    void setData(png_bytep dest, int x, int y) {
                        int horiz = y > 3 && y < 6;
                        int vert = x > 4 && x < 7;
                        dest[0] = (vert || horiz) * 255;
                        dest[1] = (vert || horiz) * 255;
                        dest[2] = !(vert || horiz) * 255;
                    }

                    setData(rows[i] + j * 3, j * 16 / width, i * 10 / height);
                }
            }

            return rows;
        }

        png_bytep rows[height];
        png_byte image[width * 3 * height];
        writeImage(stdout, getData(rows, image), width, height);
    }

    int height = 400;
    if (argc > 1) {
        sscanf(argv[1], "%d", &height);
    }

    if (height > 1200) {
        struct rlimit rl;
        getrlimit(RLIMIT_STACK, &rl);
        int rlim_cur = rl.rlim_cur;
        printf("%d\n", rlim_cur);
        rl.rlim_cur = 8720000 * 8;
        setrlimit(RLIMIT_STACK, &rl);
    }

    write_png(height / 10 * 16, height);
}
