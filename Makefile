CC = gcc
CFLAGS = -g -Wall -Werror -Wpedantic -O2
LDFLAGS =

OUTPUT = ./bin/jdir

cfiles = $(wildcard src/*.c)
objects = $(cfiles:src/%.c=bin/%.o)

install: $(OUTPUT)
	cp $(OUTPUT) ~/.local/bin/jdir

$(OUTPUT): $(objects)
	$(CC) -o $(OUTPUT) $(objects) $(CFLAGS) $(LDFLAGS)

$(objects):
	$(CC) -c $(@:bin/%.o=src/%.c) -o $@ $(CFLAGS) $(LDFLAGS)

run: $(OUTPUT)
	$(OUTPUT)

clean:
	rm bin/*

.PHONY: install run clean
