CC = gcc
CFLAGS = -g -Wall -Werror -Wpedantic
LDFLAGS =

OUTPUT = ./bin/main

cfiles = $(wildcard src/*.c)
objects = $(cfiles:src/%.c=bin/%.o)

main: $(objects)
	$(CC) -o $(OUTPUT) $(objects) $(CFLAGS) $(LDFLAGS)

$(objects):
	$(CC) -c $(@:bin/%.o=src/%.c) -o $@ $(CFLAGS) $(LDFLAGS)

run: main
	$(OUTPUT)

clean:
	rm bin/*

.PHONY: main run clean
