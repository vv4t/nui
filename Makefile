.PHONY=default run

CFLAGS=-O3
LDFLAGS=-lSDL2 -lSDL2_image -lm -lGL -lGLEW
INCLUDE=-I src/include
SRC=$(wildcard src/*/*.c)
OBJ=$(patsubst src/%.c, bin/%.o, $(SRC))
SRC_H=$(wildcard src/*/*.h)

default: nui run

nui: $(OBJ)
	gcc $(CFLAGS) $^ $(LDFLAGS) -o $@

bin/%.o: src/%.c $(SRC_H)
	mkdir -p bin
	mkdir -p "$(shell dirname "$@")"
	gcc $(INCLUDE) $(CFLAGS) -c -o $@ $<

run: nui
	./nui
