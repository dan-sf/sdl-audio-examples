#!/usr/bin/env bash

clang -L/usr/local/lib -lSDL2 -lSDL2_mixer -o wav_mix.out wav_mix.c

