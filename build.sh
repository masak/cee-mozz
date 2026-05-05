#! /bin/bash
set -e

echo 'Building src/arena.c ...'
clang -c -std=c11 -Wall -Wextra -pedantic -g -O0 -o build/arena.o src/arena.c
echo 'Building src/i64-value.c ...'
clang -c -std=c11 -Wall -Wextra -pedantic -g -O0 -o build/i64-value.o \
    src/i64-value.c
echo 'Building test/value-01-i64.c ...'
clang -c -std=c11 -Wall -Wextra -pedantic -g -O0 -o build/value-01-i64.o \
    test/value-01-i64.c

echo 'Linking ...'
clang \
    build/arena.o \
    build/i64-value.o \
    build/value-01-i64.o \
    -o build/value-01-i64

