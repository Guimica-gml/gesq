#!/bin/sh

CFLAGS="-Wall -Wextra -std=c11 -pedantic"
LIBS="-lSDL2"

gcc $CFLAGS -o gesq ./main.c $LIBS

if [ "$1" = "run" ]; then
    ./gesq
fi
