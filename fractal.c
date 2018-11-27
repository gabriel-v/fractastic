#include <complex.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h> 
#include "fractal.h"

int julia(const complex_function f,
          const double complex z0,
          const double complex c,
          const double complex d,
          const fractal_config *const fc) {
    double complex z_old = z0;
    double complex z_new = f(z_old, c, d);

    for (int i = 1; i < fc->max_iterations; i++) {
        z_old = z_new;
        z_new = f(z_old, c, d);

        if (cabs(z_new) > fc->infinity) {
            return i;
        }
    }

    return CONVERGE;
}

int generalized_mandelbrot(const complex_function f,
                           const double complex z0,
                           const double complex d,
                           const fractal_config *const fc) {
    return julia(f, 0, z0, d, fc);
}

double complex complex_polynomial(const double complex z0,
                                  const double complex c,
                                  const double complex d) {
    return cpow(z0, d) + c;
}

double complex newtonfractalfunction(const double complex z0,
                                      const double complex d) {
    return cpow(z0, d) - 1;
}

double complex newtonfractalderiv(const double complex z0,
                                    const double complex d) {
    return d * cpow(z0, d - 1);
}

void durandKerner(double complex * roots, int d) {
    srand(time(0)); 
    double complex * rootsNew = malloc (d * sizeof(double complex));
    for (int i = 0; i < d; i++) {
        double theta = rand();
        roots[i] = cos(theta) + I * sin(theta);
    }
    
    for (int k = 0; k < 1000; k++) {
        for(int i = 0; i < d; i++) {
            double complex temp = 1;
            for (int j = 0; j < d; j++)
                if (j != i)
                    temp *= roots[i] - roots[j];
            rootsNew[i] = roots[i] - newtonfractalfunction(roots[i], d) / temp;
        }
        for(int i = 0; i < d; i++) {
            roots[i] = rootsNew[i]; 
        }
    }
}

int newton(const double complex z0,
          const double complex a,
          const double complex * roots,
          const double complex d,
          const fractal_config *const fc) {
    double complex z_old = z0;
    double complex z_new;
    double tol = 0.00001;

    for (int i = 1; i < fc->max_iterations; i++) {
        z_new = z_old - a * newtonfractalfunction(z_old, d) / newtonfractalderiv(z_old, d);

        for (int j = 0; j < creal(d); j++) {
            if (cabs(z_new - roots[j]) < tol)
                return i;
        }
        z_old = z_new;
    }

    return fc->max_iterations;
}

