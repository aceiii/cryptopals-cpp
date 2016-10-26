#!/bin/sh

filename=$(basename $1)
extension="${filename##*.}"
filename="${filename%.*}"
g++ $1 -o bin/$filename -std=c++14 && bin/$filename

