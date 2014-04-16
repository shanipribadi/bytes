
cc = clang
flags = -std=c11 -Wall `pkg-config lv2 --cflags` -shared -fPIC -DPIC -g
sources = src/voice.c src/eg.c src/bytes.c src/lv2plugin.c
vstsources = $(sources) src/vst/lv2_evbuf.c src/vstplugin.c

all: lv2 vst

lv2:
	mkdir -p build/bytes.lv2
	$(cc) -Werror $(flags) $(sources) -o build/bytes.lv2/bytes.so
	cp bytes.lv2/*.ttl build/bytes.lv2

vst:
	mkdir -p build/
	$(cc) -Werror $(flags) $(vstsources) -o build/bytes-vst.so

bitwigvst:
	mkdir -p build/
	gcc -DWTF_BITWIG $(flags) $(vstsources) -o build/bytes-vst.so

install:
	cp -rf build/bytes.lv2 ~/.lv2
	cp -rf build/bytes-vst.so ~/.vst
