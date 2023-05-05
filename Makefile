CC := gcc
CFLAGS := -I src/
LIBS := -lncurses

.PHONY: all
all: build/main

.PHONY: objects
objects: $(patsubst src/%.c, build/%.o, $(wildcard src/*.c))

build/%.o: src/%.c
	mkdir -p build
	$(CC) -c $(CFLAGS) $< -o $@

build/main: build/main.o
	${CC} build/*.o ${LIBS} -o $@

.PHONY: list
list:
	@echo Valid Targets:
	@make -qp | awk -F':' '/^[a-zA-Z0-9][^$$#\/\t=]*:([^=]|$$)/ {split($$1,A,/ /);for(i in A)print A[i]}' | sort -u | grep -v "^Makefile$$" | sed 's/^/  - /g'

.PHONY: run
run: all
	./build/main

.PHONY: debug
debug: CFLAGS+=-g -Wall -Wpedantic
debug: LIBS+=
debug: all

.PHONY: release
release: CFLAGS+=-O2
release: LIBS+=
release: all

.PHONY: clean
clean:
	rm -rf build/
