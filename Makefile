.PHONY=default run build

CFLAGS=-O3 -Wall
LDFLAGS=-lSDL2 -lSDL2_image -lm
SRC=$(wildcard src/*.cpp)
OBJ=$(patsubst src/%.cpp, bin/%.o, $(SRC))
SRC_H=$(wildcard src/*.h)
INCLUDE=-Iinclude

default: nui run

nui: $(OBJ)
	g++ $(CFLAGS) $^ $(LDFLAGS) -o $@

bin/%.o: src/%.cpp $(SRC_H)
	mkdir -p bin
	g++ $(INCLUDE) $(CFLAGS) -c -o $@ $<

build:
	g++ $(CFLAGS) $(LDFLAGS) $(INCLUDE) $(SRC) -o nui

run: nui
	./nui
