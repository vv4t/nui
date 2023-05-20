.PHONY=build test

ASSETS_SRC=$(wildcard res/*/*/*.png) $(wildcard res/*/*/*.jpg) $(wildcard res/*/*.mesh) $(wildcard res/*/*.frag) $(wildcard res/*/*.vert)
ASSETS = $(patsubst %, --preload-file %, $(ASSETS_SRC))

build: src/*.c src/*/*.c src/*/*.h res/*/*
	emcc -s WASM=1 -O3 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_WEBGL2=1 $(ASSETS) --use-preload-plugins src/*.c src/*/*.c -o bin/index.html
