CC = gcc

CFLAGS = -g -Wall -O3 -std=c99 
LDFLAGS = -lm -lrt

fractastic: fractal.o fractastic.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) -fopenmp

fractastic.o: fractastic.c
	$(CC) $(CFLAGS) -c $^ -o $@ $(LDFLAGS) -fopenmp
fractastic.c: fractal.h

fractal.o: fractal.c
	$(CC) $(CFLAGS) -c $^ -o $@ 
fractal.c: fractal.h

.PHONY: clean
clean:
	rm -f *.o fractastic

.PHONY: all
	all: clean fractastic

