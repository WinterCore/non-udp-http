OS := $(shell uname)
CFLAGS = -std=c11 -Wall -Wextra

.PHONY: clean debug all


debug: CFLAGS += -ggdb -DDEBUG
debug: Server

release: CFLAGS += -DNDEBUG

SRC_C=$(shell find . -name "*.c")
SRC_ALL=$(shell find . -name "*.c" -o -name '*.h')

Server: $(SRC_ALL)
	cc $(CFLAGS) -o Server $(SRC_C) $(LDFLAGS)

all: Server

clean:
	rm -rf Server ./src/*.o ./src/**/*.o ./Server.dSYM

