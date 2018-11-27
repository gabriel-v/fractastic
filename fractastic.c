#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <complex.h>
#include <pthread.h>

#include "fractal.h"

#define UNKNOWN_MODE 0
#define JULIA_MODE 1
#define MANDELBROT_MODE 2
#define NEWTON_MODE 3

typedef struct {
    int min_height;
    int max_height;
    int width;
    double x_min;
    double x_max;
    double y_min;
    double y_max;
    int max_iterations;
    int color_multiplier;
    complex a;
    double c;
    complex d;
    int mode;
    double x_step;
    double y_step;
    fractal_config fc;
    unsigned char **img;
} thread_arg;

void *thread_func(void *params) {
    thread_arg *args = (thread_arg *)params;
    double x0, y0;
    double complex z0;
    int iterations = 0, color = 0;
    double complex *roots = NULL;

    if (args->mode == NEWTON_MODE) {
        roots = malloc(args->d * sizeof(double complex));
        durandKerner(roots, args->d);
    }

    for (int row = args->min_height; row < args->max_height; row++) {
        for (int col = 0; col < args->width; col++) {
            x0 = args->x_min + col * args->x_step;
            y0 = args->y_max - row * args->y_step;
            z0 = x0 + y0 * I;

            if (args->mode == JULIA_MODE) {
                iterations = julia(complex_polynomial,
                                    z0,
                                    args->c,
                                    args->d,
                                    &(args->fc));
            } else if (args->mode == MANDELBROT_MODE) {
                iterations = generalized_mandelbrot(complex_polynomial,
                                                    z0,
                                                    args->d,
                                                    &(args->fc));
            } else if (args->mode == NEWTON_MODE) {
                iterations = newton(z0,
                                    args->a,
                                    roots,
                                    args->d,
                                    &(args->fc));
            }

            if (iterations == CONVERGE) {
                color = 255;
            } else if(args->mode == NEWTON_MODE){
                color = 255 * iterations * args->color_multiplier / (float)args->max_iterations;
            } else {
                color = args->color_multiplier * iterations;
            }

            color = color < 0 ? 0 : color;
            color = color > 255 ? 255 : color;
            args->img [row][col] = color;
        }
    }

    return NULL;
}

void init_thread_arg(thread_arg *arg, int min_height, int max_height, int width,
        double x_min, double x_max, double y_min, double y_max,
        int max_iterations, int color_multiplier, double d,
        complex a, complex c, int mode, double x_step, 
        double y_step, fractal_config fc, unsigned char **img) {
    arg->min_height = min_height;
    arg->max_height = max_height;
    arg->width = width;
    arg->x_min = x_min;
    arg->x_max = x_max;
    arg->y_min = y_min;
    arg->y_max = y_max;
    arg->max_iterations = max_iterations;
    arg->color_multiplier = color_multiplier;
    arg->a = a;
    arg->c = c;
    arg->d = d;
    arg->mode = mode;
    arg->x_step = x_step;
    arg->y_step = y_step;
    arg->fc = fc;
    arg->img = img;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s [output_file.pgm] [J/M/N] [options]\n", argv[0]);
        return 1;
    } else {
        int mode = UNKNOWN_MODE;
        char * filename = argv[1];

        if (strcmp(argv[2], "J") == 0) {
            mode = JULIA_MODE;
        } else if (strcmp(argv[2], "M") == 0) {
            mode = MANDELBROT_MODE;
        } else if (strcmp(argv[2], "N") == 0) {
            mode = NEWTON_MODE;
        }

        if (mode == UNKNOWN_MODE) {
            fprintf(stderr, "Unrecognized mode \"%s\"", argv[2]);
            return 2;
        } else if (mode == JULIA_MODE && argc != 15) {
            fprintf(stderr, "Usage: %s J [width] [height] [x_min] [x_max] [y_min] [y_max] [max_iterations] [color_multiplier] [c_re] [c_im] [d] [thread_num]\n", argv[0]);
            return 3;
        } else if (mode == MANDELBROT_MODE && argc != 13) {
            fprintf(stderr, "Usage: %s M [width] [height] [x_min] [x_max] [y_min] [y_max] [max_iterations] [color_multiplier] [d] [thread_num]\n", argv[0]);
            return 4;
        } else if (mode == NEWTON_MODE && argc != 15) {
            fprintf(stderr, "Usage: %s N [width] [height] [x_min] [x_max] [y_min] [y_max] [max_iterations] [color_multiplier] [a_re] [a_im] [d] [thread_num]\n", argv[0]);
            return 5;
        }

        int arg = 3;

        const int width = strtol(argv[arg++], NULL, 0);
        const int height = strtol(argv[arg++], NULL, 0);
        const double x_min = strtod(argv[arg++], NULL);
        const double x_max = strtod(argv[arg++], NULL);
        const double y_min = strtod(argv[arg++], NULL);
        const double y_max = strtod(argv[arg++], NULL);
        const int max_iterations = strtol(argv[arg++], NULL, 0);
        const int color_multiplier = strtol(argv[arg++], NULL, 0);

        const double c_re = mode == JULIA_MODE ? strtod(argv[arg++], NULL) : 0;
        const double c_im = mode == JULIA_MODE ? strtod(argv[arg++], NULL) : 0;
        const double a_re = mode == NEWTON_MODE ? strtod(argv[arg++], NULL) : 0;
        const double a_im = mode == NEWTON_MODE ? strtod(argv[arg++], NULL) : 0;
        const double d = strtod(argv[arg++], NULL);
        int thread_num = atoi(argv[arg++]);

        const double x_step = (x_max - x_min) / width;
        const double y_step = (y_max - y_min) / height;

        const fractal_config fc = {EPSILON, INFINITY, max_iterations};

        const double complex c = c_re + c_im * I;
        const double complex a = a_re + a_im * I;

        double complex * roots = malloc(d * sizeof(double complex));;
        if (mode == NEWTON_MODE) 
            durandKerner(roots, d);

        unsigned char ** img;
        img = calloc(height, sizeof(unsigned char *));
        for (int i = 0; i < height; i++)
            img[i] = calloc(width, sizeof(unsigned char));

        thread_arg *args = malloc(thread_num * sizeof(thread_arg));
        if (!args) {
            return 1;
        }

        pthread_t *threads = malloc(thread_num * sizeof(pthread_t));
        if (!threads) {
            return 1;
        }

        for (int i = 0; i < thread_num; i++) {
            int min_height = height / thread_num * i;
            int max_height = min_height + height / thread_num;
            init_thread_arg(&args[i], min_height, max_height, width,
                x_min, x_max, y_min, y_max, max_iterations, color_multiplier, 
                d, a, c, mode, x_step, y_step, fc, img);
            if (pthread_create(&threads[i], NULL, &thread_func, &args[i])) {
                return 1;
            }
        }

        for (int i = 0; i < thread_num; i++) {
            pthread_join(threads[i], NULL);
        }

        free(args);
        free(threads);

        FILE * f = fopen(filename, "wb");
        fprintf(f, "P5\n%d %d\n255\n", width, height);

        for (int row = 0; row < height; row++) {
            fwrite(img[row], width, 1, f);
        }
        fclose(f);

        return 0;
    }
}
