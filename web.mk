.PHONY=build

ASSETS_PNG=$(wildcard res/*/*/*.png)
ASSETS_JPG=$(wildcard res/*/*/*.jpg)
ASSETS_MESH=$(wildcard res/*/*.mesh)
ASSETS_SHADERS=$(wildcard res/*/*.frag) $(wildcard res/*/*.vert)
ASSETS_SRC=$(ASSETS_PNG) $(ASSETS_JPG) $(ASSETS_MESH) $(ASSETS_SHADERS)
ASSETS=$(patsubst %, --preload-file %, $(ASSETS_SRC)) --use-preload-plugins

OUT=bin/index.html
CFLAGS=-s WASM=1 -O3
LDFLAGS=-s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_WEBGL2=1
SRC=src/*.c src/*/*.c

build: src/*.c src/*/*.c src/*/*.h res/*/*
	emcc $(CFLAGS) $(LDFLAGS) $(ASSETS) $(SRC) -o $(OUT)
