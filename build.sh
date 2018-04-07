#!/usr/bin/env bash

clang -L/usr/local/lib -lSDL2 -lSDL2_mixer -o wav_mix.out wav_mix.c
clang -L/usr/local/lib -lSDL2 -o wav_callback.out wav_callback.c

