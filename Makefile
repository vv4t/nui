.PHONY=default run

CFLAGS=-O3
LDFLAGS=-lSDL2 -lSDL2_image -lm -lGL -lGLEW
SRC=src/*.c src/*/*.c

default: nui run

nui: $(SRC) src/*/*.h
	gcc $(CFLAGS) $(SRC) $(LDFLAGS) -o $@

run:
	./nui
