
cc = gcc
flags = -std=c99 -Wall `pkg-config lv2 --cflags` -shared -ffast-math -O3 -fPIC -DPIC -g
sources = src/eg.c src/bytes.c src/lv2plugin.c

all: lv2

lv2:
	mkdir -p build/bytes.lv2
	$(cc) -Werror $(flags) $(sources) -o build/bytes.lv2/bytes.so
	cp bytes.lv2/*.ttl build/bytes.lv2

install:
	cp -rf build/bytes.lv2 ~/.lv2
	cp -rf build/bytes-vst.so ~/.vst
