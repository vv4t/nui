.PHONY=build

ASSETS_PNG=$(wildcard assets/*/*/*.png)
ASSETS_JPG=$(wildcard assets/*/*/*.jpg)
ASSETS_MDL=$(wildcard assets/*/*/*.mdl)
ASSETS_MAP=$(wildcard assets/*/*/*.map)
ASSETS_FRAG=$(wildcard assets/*/*/*.frag)
ASSETS_VERT= $(wildcard assets/*/*/*.vert)
ASSETS_GLSL=$(wildcard assets/*/*/*.glsl)
ASSETS_GLSL=$(wildcard assets/*/*/*.glsl)
ASSETS_TTF=$(wildcard assets/*/*.ttf)
ASSETS_SRC=$(ASSETS_PNG) $(ASSETS_JPG) $(ASSETS_MDL) $(ASSETS_MAP) $(ASSETS_VERT) $(ASSETS_FRAG) $(ASSETS_GLSL) $(ASSETS_TTF)
ASSETS=$(patsubst %, --preload-file %, $(ASSETS_SRC)) --use-preload-plugins

OUT=bin/web/index.html
CFLAGS=-s WASM=1 -O3
LDFLAGS=-s USE_SDL=2 -s USE_SDL_IMAGE=2 -s USE_WEBGL2=1 -s USE_FREETYPE=1
SRC=src/*/*.c

build: src/*/*.c src/*/*.h assets/*/*/*
	emcc $(CFLAGS) $(LDFLAGS) $(ASSETS) $(SRC) -o $(OUT)
