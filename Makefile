.POSIX:

CC      = clang
CFLAGS  = -std=c11 -Wall -Wextra -pedantic -g -O0 -fsanitize=address,undefined
LDFLAGS =

SRC_OBJS = \
 build/arena.o \
 build/array-value.o \
 build/ascii-str-value.o \
 build/code-table.o \
 build/codeunit.o \
 build/crash.o \
 build/environment.o \
 build/func-value.o \
 build/i64-value.o \
 build/int-table.o \
 build/int-value.o \
 build/macro-value.o \
 build/parameters.o \
 build/seenset.o \
 build/str-table.o \
 build/str-value.o \
 build/syntax-node-value.o \
 build/value.o

TEST_EXECUTABLES = \
 build/value-01-i64 \
 build/value-02-ascii-str \
 build/value-06-array \
 build/value-07-func \
 build/value-08-int \
 build/value-09-str \
 build/value-10-syntax-node

all: $(TEST_EXECUTABLES)

test: all
	./build/value-01-i64
	./build/value-02-ascii-str
	./build/value-06-array
	./build/value-07-func
	./build/value-08-int
	./build/value-09-str
	./build/value-10-syntax-node

# -----------------------------------------------------------------------------
# Test executables
# -----------------------------------------------------------------------------

build/value-01-i64: build/value-01-i64.o $(SRC_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ build/value-01-i64.o $(SRC_OBJS)

build/value-02-ascii-str: build/value-02-ascii-str.o $(SRC_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ build/value-02-ascii-str.o $(SRC_OBJS)

build/value-06-array: build/value-06-array.o $(SRC_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ build/value-06-array.o $(SRC_OBJS)

build/value-07-func: build/value-07-func.o $(SRC_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ build/value-07-func.o $(SRC_OBJS)

build/value-08-int: build/value-08-int.o $(SRC_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ build/value-08-int.o $(SRC_OBJS)

build/value-09-str: build/value-09-str.o $(SRC_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ build/value-09-str.o $(SRC_OBJS)

build/value-10-syntax-node: build/value-10-syntax-node.o $(SRC_OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ build/value-10-syntax-node.o $(SRC_OBJS)

# -----------------------------------------------------------------------------
# Source objects
# -----------------------------------------------------------------------------

build/arena.o: source/arena.c include/arena.h include/crash.h include/typedefs.h
	$(CC) $(CFLAGS) -c -o $@ source/arena.c

build/array-value.o: source/array-value.c include/arena.h include/array-value.h include/crash.h include/i64-value.h include/seenset.h include/tags.h include/typedefs.h include/value.h
	$(CC) $(CFLAGS) -c -o $@ source/array-value.c

build/ascii-str-value.o: source/ascii-str-value.c include/arena.h include/ascii-str-value.h include/crash.h include/i64-value.h include/seenset.h include/tags.h include/typedefs.h include/value.h
	$(CC) $(CFLAGS) -c -o $@ source/ascii-str-value.c

build/code-table.o: source/code-table.c include/arena.h include/code-table.h include/crash.h include/seenset.h include/tags.h include/typedefs.h include/value.h
	$(CC) $(CFLAGS) -c -o $@ source/code-table.c

build/codeunit.o: source/codeunit.c include/arena.h include/code-table.h include/codeunit.h include/crash.h include/environment.h include/int-table.h include/parameters.h include/seenset.h include/str-table.h include/tags.h include/typedefs.h include/value.h
	$(CC) $(CFLAGS) -c -o $@ source/codeunit.c

build/crash.o: source/crash.c include/crash.h
	$(CC) $(CFLAGS) -c -o $@ source/crash.c

build/environment.o: source/environment.c include/arena.h include/crash.h include/environment.h include/seenset.h include/tags.h include/typedefs.h include/value.h
	$(CC) $(CFLAGS) -c -o $@ source/environment.c

build/func-value.o: source/func-value.c include/arena.h include/codeunit.h include/crash.h include/environment.h include/func-value.h include/parameters.h include/seenset.h include/tags.h include/typedefs.h include/value.h
	$(CC) $(CFLAGS) -c -o $@ source/func-value.c

build/i64-value.o: source/i64-value.c include/arena.h include/crash.h include/i64-value.h include/seenset.h include/tags.h include/typedefs.h include/value.h
	$(CC) $(CFLAGS) -c -o $@ source/i64-value.c

build/int-table.o: source/int-table.c include/arena.h include/crash.h include/int-table.h include/seenset.h include/tags.h include/typedefs.h include/value.h
	$(CC) $(CFLAGS) -c -o $@ source/int-table.c

build/int-value.o: source/int-value.c include/arena.h include/ascii-str-value.h include/crash.h include/int-value.h include/seenset.h include/tags.h include/typedefs.h include/value.h
	$(CC) $(CFLAGS) -c -o $@ source/int-value.c

build/macro-value.o: source/macro-value.c include/arena.h include/codeunit.h include/crash.h include/environment.h include/macro-value.h include/parameters.h include/seenset.h include/tags.h include/typedefs.h include/value.h
	$(CC) $(CFLAGS) -c -o $@ source/macro-value.c

build/parameters.o: source/parameters.c include/arena.h include/crash.h include/parameters.h include/seenset.h include/tags.h include/typedefs.h include/value.h
	$(CC) $(CFLAGS) -c -o $@ source/parameters.c

build/seenset.o: source/seenset.c include/arena.h include/crash.h include/seenset.h include/typedefs.h
	$(CC) $(CFLAGS) -c -o $@ source/seenset.c

build/str-table.o: source/str-table.c include/arena.h include/crash.h include/seenset.h include/str-table.h include/tags.h include/typedefs.h include/value.h
	$(CC) $(CFLAGS) -c -o $@ source/str-table.c

build/str-value.o: source/str-value.c include/arena.h include/crash.h include/seenset.h include/str-value.h include/tags.h include/typedefs.h include/value.h
	$(CC) $(CFLAGS) -c -o $@ source/str-value.c

build/syntax-node-value.o: source/syntax-node-value.c include/arena.h include/crash.h include/seenset.h include/syntax-node-value.h include/tags.h include/typedefs.h include/value.h
	$(CC) $(CFLAGS) -c -o $@ source/syntax-node-value.c

build/value.o: source/value.c include/arena.h include/array-value.h include/ascii-str-value.h include/code-table.h include/codeunit.h include/crash.h include/environment.h include/func-value.h include/i64-value.h include/int-table.h include/int-value.h include/macro-value.h include/parameters.h include/seenset.h include/str-table.h include/str-value.h include/tags.h include/typedefs.h include/value.h
	$(CC) $(CFLAGS) -c -o $@ source/value.c

# -----------------------------------------------------------------------------
# Test objects
# -----------------------------------------------------------------------------

build/value-01-i64.o: test/value-01-i64.c include/arena.h include/i64-value.h include/seenset.h include/test.h include/typedefs.h
	$(CC) $(CFLAGS) -c -o $@ test/value-01-i64.c

build/value-02-ascii-str.o: test/value-02-ascii-str.c include/arena.h include/ascii-str-value.h include/i64-value.h include/seenset.h include/tags.h include/test.h include/typedefs.h include/value.h
	$(CC) $(CFLAGS) -c -o $@ test/value-02-ascii-str.c

build/value-06-array.o: test/value-06-array.c include/arena.h include/array-value.h include/i64-value.h include/seenset.h include/tags.h include/test.h include/typedefs.h include/value.h
	$(CC) $(CFLAGS) -c -o $@ test/value-06-array.c

build/value-07-func.o: test/value-07-func.c include/arena.h include/array-value.h include/ascii-str-value.h include/codeunit.h include/environment.h include/func-value.h include/i64-value.h include/int-table.h include/macro-value.h include/parameters.h include/seenset.h include/str-table.h include/tags.h include/test.h include/typedefs.h include/value.h
	$(CC) $(CFLAGS) -c -o $@ test/value-07-func.c

build/value-08-int.o: test/value-08-int.c include/arena.h include/ascii-str-value.h include/int-value.h include/seenset.h include/tags.h include/test.h include/typedefs.h
	$(CC) $(CFLAGS) -c -o $@ test/value-08-int.c

build/value-09-str.o: test/value-09-str.c include/arena.h include/seenset.h include/str-value.h include/tags.h include/test.h include/typedefs.h include/value.h
	$(CC) $(CFLAGS) -c -o $@ test/value-09-str.c

build/value-10-syntax-node.o: test/value-10-syntax-node.c include/arena.h include/ascii-str-value.h include/i64-value.h include/seenset.h include/syntax-node-value.h include/tags.h include/test.h include/typedefs.h include/value.h
	$(CC) $(CFLAGS) -c -o $@ test/value-10-syntax-node.c

# -----------------------------------------------------------------------------
# Cleanup
# -----------------------------------------------------------------------------

clean:
	rm -f build/*.o $(TEST_EXECUTABLES)

