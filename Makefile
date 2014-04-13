
cc = clang

all:
	mkdir -p build/bytes.lv2
	$(cc) -std=c11 -Wall -Werror `pkg-config lv2 --cflags` -shared -fPIC -DPIC src/*.c -o build/bytes.lv2/bytes.so -g
	cp bytes.lv2/*.ttl build/bytes.lv2

install:
	cp -rf build/bytes.lv2 ~/.lv2
