#!/bin/bash
gcc -shared -o client/context.so -fPIC client/context.c -lGLEW -lz -lGL -lm $(pkg-config --cflags --libs sdl2 SDL2_image)