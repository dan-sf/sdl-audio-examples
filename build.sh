#!/usr/bin/env bash

args=$#

if [[ $args == 0 ]]
then
    clang -L/usr/local/lib -lSDL2 -lSDL2_mixer -o wav_mix.out wav_mix.c
    clang -L/usr/local/lib -lSDL2 -o wav_callback.out wav_callback.c
    clang -L/usr/local/lib -lSDL2 -o wav_queue.out wav_queue.c
    clang -L/usr/local/lib -lSDL2 -o wav_square.out wav_square.c
    clang -L/usr/local/lib -lSDL2 -o wav_triangle.out wav_triangle.c
    clang -L/usr/local/lib -lSDL2 -o wav_sawtooth.out wav_sawtooth.c
    clang -L/usr/local/lib -lSDL2 -o wav_sine.out wav_sine.c
elif [[ $args == 1 ]];then
    fname=$1
    if [[ $fname != *"mix"* ]]
    then
        clang -L/usr/local/lib -lSDL2 -o ${fname}.out ${fname}.c
    else
        clang -L/usr/local/lib -lSDL2 -lSDL2_mixer -o ${fname}.out ${fname}.c
    fi
else
    echo "Error: extra args provided"
    echo ""
    echo "Usage: ./build.sh [file_name (no extension)]"
    echo ""
    exit 1
fi

