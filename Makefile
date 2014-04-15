
cc = clang
sources = src/voice.c src/eg.c src/bytes.c src/lv2plugin.c

all: lv2 vst

lv2:
	mkdir -p build/bytes.lv2
	$(cc) -std=c99 -Wall -Werror `pkg-config lv2 --cflags` -shared -fPIC -DPIC $(sources) -o build/bytes.lv2/bytes.so -g
	cp bytes.lv2/*.ttl build/bytes.lv2

vst:
	mkdir -p build/
	$(cc) -std=c11 -Wall -Werror `pkg-config lv2 --cflags` -shared -fPIC -DPIC $(sources) src/vst/lv2_evbuf.c src/vstplugin.c -o build/bytes-vst.so -g

install:
	cp -rf build/bytes.lv2 ~/.lv2
	cp -rf build/bytes-vst.so ~/.vst
