#!/bin/bash

if [ $# -eq 0 ]; then
    echo "Usage: ./run.sh [output_file.ppm] [fractastic_options]"
    exit 1
fi

# Compile the program
make

# If there was an error with compilation, exit
if [ $? -ne 0 ]; then
    exit 2
fi

output_file=$1

if [ -f $output_file  ]; then
    rm $output_file
fi

# Run the program and save the output to a temporary PPM file
./fractastic $@ 

# If there was an error with the program, exit
if [ $? -ne 0 ]; then
    rm $output_file
    exit 3
fi

png_file=$(echo $output_file | cut -d '.' -f 1)

# Convert the ppm to png, if possible
if [ "$(command -v convert)" ]; then
    convert $output_file $png_file.png
    rm $output_file
else
    echo "Install ImageMagick to get png output."
fi
