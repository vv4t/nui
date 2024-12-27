.PHONY=default run build

CFLAGS=-O3 -Wall
LDFLAGS=-lSDL2 -lSDL2_image -lm
SRC=$(wildcard src/*/*.cpp)
OBJ=$(patsubst src/%.cpp, bin/%.o, $(SRC))
SRC_HPP=$(wildcard src/*/*.hpp)
INCLUDE=-Iinclude -Isrc

default: nui run

nui: $(OBJ)
	g++ $(CFLAGS) $^ $(LDFLAGS) -o $@

bin/%.o: src/%.cpp $(SRC_HPP)
	mkdir -p bin
	mkdir -p "$(shell dirname "$@")"
	gcc $(INCLUDE) $(CFLAGS) -c -o $@ $<

build:
	g++ $(CFLAGS) $(LDFLAGS) $(INCLUDE) $(SRC) -o nui

clean:
	rm -f $(OBJ)

run: nui
	./nui
