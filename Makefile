CC = gcc

CFLAGS = -g -Wall -O3 -fopenmp --std=c99
LDFLAGS = -g -lm -fopenmp


.PHONY: all

all: clean fractastic main2 main3

fractastic: ppm.o fractal.o fractastic.o
main2: ppm.o fractal.o main2.o
main3: ppm.o fractal.o main3.o

fractastic.o: fractastic.c
main2.o: main2.c
main3.o: main3.c
fractastic.c: ppm.h fractal.h

ppm.o: ppm.c
ppm.c: ppm.h

fractal.o: fractal.c
fractal.c: fractal.h

.PHONY: clean
clean:
	rm -f *.o fractastic fractastic-omp-simple

