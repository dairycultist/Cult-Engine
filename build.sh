#!/bin/bash
gcc -shared -o client/contextlib.so -fPIC client/contextlib.c -lGLEW -lGL -lm $(pkg-config --cflags --libs sdl2 SDL2_image)