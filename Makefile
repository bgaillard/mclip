CC=gcc

all: mclip

clean:
	rm -rf build

build: main.c
	mkdir -p build
	$(CC) -o build/mclip main.c

install: build
	cp build/mclip /usr/bin
