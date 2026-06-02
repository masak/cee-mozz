#! /bin/bash
set -e

build_c_file() {
    local src="$1"
    local name="${src##*/}"
    name="${name%.c}"
    local obj="build/${name}.o"

    echo "Building ${src} ..."
    clang -c -std=c11 -Wall -Wextra -pedantic -g -O0 -o "${obj}" "${src}"
}

build_c_file "src/arena.c"
build_c_file "src/value.c"
build_c_file "src/i64-value.c"
build_c_file "src/ascii-str-value.c"
build_c_file "test/value-01-i64.c"

echo 'Linking ...'
clang \
    build/arena.o \
    build/value.o \
    build/i64-value.o \
    build/ascii-str-value.o \
    build/value-01-i64.o \
    -o build/value-01-i64

