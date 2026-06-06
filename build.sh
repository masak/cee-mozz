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
build_c_file "src/array-value.c"
build_c_file "src/func-value.c"
build_c_file "src/environment.c"
build_c_file "src/codeunit.c"
build_c_file "test/value-01-i64.c"
build_c_file "test/value-02-ascii-str.c"
build_c_file "test/value-06-array.c"

echo 'Linking ...'
clang \
    build/arena.o \
    build/value.o \
    build/i64-value.o \
    build/ascii-str-value.o \
    build/array-value.o \
    build/func-value.o \
    build/environment.o \
    build/codeunit.o \
    build/value-01-i64.o \
    -o build/value-01-i64
clang \
    build/arena.o \
    build/value.o \
    build/i64-value.o \
    build/ascii-str-value.o \
    build/array-value.o \
    build/func-value.o \
    build/environment.o \
    build/codeunit.o \
    build/value-02-ascii-str.o \
    -o build/value-02-ascii-str
clang \
    build/arena.o \
    build/value.o \
    build/i64-value.o \
    build/ascii-str-value.o \
    build/array-value.o \
    build/func-value.o \
    build/environment.o \
    build/codeunit.o \
    build/value-06-array.o \
    -o build/value-06-array

