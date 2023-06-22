#!/usr/bin/env sh

set -e

CFLAGS="-Wall -Wextra -std=c11 -pedantic"
LIBS="-lraylib"

gcc $CFLAGS -o gesq ./src/main.c ./src/gol.c ./src/ui.c $LIBS

if [ "$1" = "run" ]; then
    ./gesq
fi
