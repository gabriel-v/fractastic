CC = gcc

CFLAGS = -g -Wall -O3 -fopenmp
LDFLAGS = -g -lm -fopenmp


.PHONY: all

all: clean fractastic main2

fractastic: ppm.o fractal.o fractastic.o
main2: ppm.o fractal.o main2.o

fractastic.o: fractastic.c
main2.o: main2.c
fractastic.c: ppm.h fractal.h

ppm.o: ppm.c
ppm.c: ppm.h

fractal.o: fractal.c
fractal.c: fractal.h

.PHONY: clean
clean:
	rm -f *.o fractastic fractastic-omp-simple

