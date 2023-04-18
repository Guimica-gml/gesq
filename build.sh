#!/bin/sh

CFLAGS="-Wall -Wextra -std=c11 -pedantic"
LIBS="-lSDL2 -lSDL2_ttf"

gcc $CFLAGS -o gesq ./src/main.c ./src/ui.c ./src/gol.c $LIBS
RESULT=$?

if [ $RESULT -eq 0 ]; then
    if [ "$1" = "run" ]; then
        ./gesq
    fi
fi
