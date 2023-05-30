.PHONY=nui run

CFLAGS=-O3
LDFLAGS=-lSDL2 -lSDL2_image -lm -lGL -lGLEW
SRC=src/*.c src/*/*.c

nui: bin/linux/nui run

bin/linux/nui: $(SRC) src/*/*.h | bin/linux
	gcc $(CFLAGS) $(SRC) $(LDFLAGS) -o $@

bin/linux: | bin
	mkdir -p bin/linux

bin:
	mkdir -p bin

run:
	bin/linux/nui
