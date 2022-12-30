build_library: source/http_server.c
	mkdir -p bin/lib
	gcc -c source/http_server.c -fPIC -o bin/http_server.o
	gcc -shared bin/http_server.o -o bin/lib/libhttpserver.so

install:
	sudo cp bin/lib/libhttpserver.so /usr/lib
	sudo chmod 0755 /usr/lib/libhttpserver.so
	sudo ldconfig

example:
	mkdir -p bin
	gcc -g -O0 -Wall -Iinclude -lhttpserver examples/example_server.c -o bin/server


