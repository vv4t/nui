.PHONY=nui run

OUT=bin/linux/nui
CFLAGS=
LDFLAGS=-lSDL2 -lSDL2_image -lm -lGL -lGLEW
SRC=src/*.c src/*/*.c

nui: $(OUT) run

$(OUT): $(SRC) src/*/*.h
	gcc $(CFLAGS) $(SRC) $(LDFLAGS) -o $(OUT)

run:
	$(OUT)
