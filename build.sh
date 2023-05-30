#!/bin/sh

set -e

CFLAGS="-Wall -Wextra -std=c11 -pedantic"
LIBS="-lSDL2 -lSDL2_ttf"

gcc $CFLAGS -o gesq ./src/main.c ./src/ui.c ./src/gol.c $LIBS

if [ "$1" = "run" ]; then
    ./gesq
fi
