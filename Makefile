.PHONY=default run

CFLAGS=-O3
LDFLAGS=-lSDL2 -lSDL2_image -lm -lGL -lGLEW -lfreetype -pthread
INCLUDE=-I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/harfbuzz -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/sysprof-6
SRC=$(wildcard src/*/*.c)
OBJ=$(patsubst src/%.c, bin/%.o, $(SRC))
SRC_H=$(wildcard src/*/*.h)

defualt: nui run

nui: $(OBJ)
	gcc $(CFLAGS) $(LDFLAGS) $^ -o $@

bin/%.o: src/%.c $(SRC_H) | bin/common/ bin/game/ bin/client/ bin/renderer/ bin/pipeline/ bin/gl/ bin/ngui/
	gcc $(INCLUDE) $(CFLAGS) -c -o $@ $<

bin/common/: | bin/
	mkdir -p $@

bin/game/: | bin/
	mkdir -p $@

bin/client/: | bin/
	mkdir -p $@

bin/renderer/: | bin/
	mkdir -p $@

bin/pipeline/: | bin/
	mkdir -p $@

bin/gl/: | bin/
	mkdir -p $@

bin/ngui/: | bin/
	mkdir -p $@

bin/:
	mkdir -p $@

run:
	./nui
