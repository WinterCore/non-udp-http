OS := $(shell uname)
CFLAGS = -std=c11 -Wall -Wextra

.PHONY: clean debug all

release: CFLAGS += -DNDEBUG
release: Server

debug: CFLAGS += -ggdb -O0 -DDEBUG
debug: Server

SRC_C=$(shell find . -name "*.c")
SRC_ALL=$(shell find . -name "*.c" -o -name '*.h')

Server: $(SRC_ALL)
	cc $(CFLAGS) -o Server $(SRC_C) $(LDFLAGS)

all: Server

clean:
	rm -rf Server ./src/*.o ./src/**/*.o ./Server.dSYM
