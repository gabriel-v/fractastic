#!/bin/bash

make

if [ $? -ne 0 ]; then
    exit 2
fi

rm fractal.data


for thread_num in 1 2 4 8 12 16 20 24;
do
    if [ $1 == "J" ] 
    then
    ( time ./fractastic fractal.pgm $1 $2 $2 -2 2 -2 2 $3 20 -0.618 0 2 $thread_num ) 2>>temp.data
    fi
    if [ $1 == "M" ] 
    then
    ( time ./fractastic fractal.pgm $1 $2 $2 -2.5 1.5 -2 2 $3 20 2 $thread_num ) 2>>temp.data
    fi
    if [ $1 == "N" ] 
    then
    ( time ./fractastic fractal.pgm $1 $2 $2 -2 2 -2 2 $3 20 1 0 8 $thread_num ) 2>>temp.data
    fi
done

rm fractal.pgm

cat temp.data | grep real | cut -f 2 > fractal.data

rm temp.data

python graph.py fractal.data