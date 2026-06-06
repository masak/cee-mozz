.POSIX:

CC		= clang
CFLAGS  = -std=c11 -Wall -Wextra -pedantic -g -O0
LDFLAGS =

SRC_OBJS = \
 build/arena.o \
 build/value.o \
 build/i64-value.o \
 build/ascii-str-value.o \
 build/array-value.o \
 build/func-value.o \
 build/environment.o \
 build/codeunit.o

TEST_EXECUTABLES = \
 build/value-01-i64 \
 build/value-02-ascii-str \
 build/value-06-array

all: $(TEST_EXECUTABLES)

test: all
	./build/value-01-i64
	./build/value-02-ascii-str
	./build/value-06-array

# -----------------------------------------------------------------------------
# Test executables
# -----------------------------------------------------------------------------

build/value-01-i64: build/value-01-i64.o $(SRC_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ build/value-01-i64.o $(SRC_OBJS)

build/value-02-ascii-str: build/value-02-ascii-str.o $(SRC_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ build/value-02-ascii-str.o $(SRC_OBJS)

build/value-06-array: build/value-06-array.o $(SRC_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ build/value-06-array.o $(SRC_OBJS)

# -----------------------------------------------------------------------------
# Source objects
# -----------------------------------------------------------------------------

build/arena.o: src/arena.c include/arena.h
	$(CC) $(CFLAGS) -c -o $@ src/arena.c

build/value.o: src/value.c include/arena.h include/value.h include/typedefs.h
	$(CC) $(CFLAGS) -c -o $@ src/value.c

build/i64-value.o: src/i64-value.c include/arena.h include/i64-value.h include/tags.h
	$(CC) $(CFLAGS) -c -o $@ src/i64-value.c

build/ascii-str-value.o: src/ascii-str-value.c include/arena.h include/ascii-str-value.h include/i64-value.h include/typedefs.h include/tags.h include/value.h
	$(CC) $(CFLAGS) -c -o $@ src/ascii-str-value.c

build/array-value.o: src/array-value.c include/array-value.h include/arena.h include/i64-value.h include/tags.h
	$(CC) $(CFLAGS) -c -o $@ src/array-value.c

build/func-value.o: src/func-value.c include/arena.h include/codeunit.h include/environment.h include/func-value.h include/tags.h
	$(CC) $(CFLAGS) -c -o $@ src/func-value.c

build/environment.o: src/environment.c include/arena.h include/environment.h include/typedefs.h include/tags.h
	$(CC) $(CFLAGS) -c -o $@ src/environment.c

build/codeunit.o: src/codeunit.c include/arena.h include/codeunit.h include/environment.h include/func-value.h include/tags.h
	$(CC) $(CFLAGS) -c -o $@ src/codeunit.c

# -----------------------------------------------------------------------------
# Test objects
# -----------------------------------------------------------------------------

build/value-01-i64.o: test/value-01-i64.c include/i64-value.h include/test.h
	$(CC) $(CFLAGS) -c -o $@ test/value-01-i64.c

build/value-02-ascii-str.o: test/value-02-ascii-str.c include/arena.h include/ascii-str-value.h include/i64-value.h include/test.h include/typedefs.h include/tags.h
	$(CC) $(CFLAGS) -c -o $@ test/value-02-ascii-str.c

build/value-06-array.o: test/value-06-array.c include/arena.h include/array-value.h include/i64-value.h include/test.h include/tags.h include/value.h
	$(CC) $(CFLAGS) -c -o $@ test/value-06-array.c

# -----------------------------------------------------------------------------
# Cleanup
# -----------------------------------------------------------------------------

clean:
	rm -f build/*.o $(TEST_EXECUTABLES)

