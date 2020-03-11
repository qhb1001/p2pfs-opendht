.PHONY: all test clean data
data: data/main.cpp
	g++ data/main.cpp -std=c++14 -lopendht -lgnutls -o data/main
	./data/main

test_getattr: test/test_getattr.cpp
	g++ test/test_getattr.cpp -std=c++14 -lopendht -lgnutls `pkg-config fuse --cflags --libs` -lstdc++ -o test/test_getattr

main: src/nbfs.h src/operations.cpp src/helpers.cpp src/main.cpp
	g++ src/operations.cpp src/helpers.cpp src/main.cpp -std=c++14 -g -lpthread -lopendht -lgnutls `pkg-config fuse --cflags --libs` -lstdc++ -o src/fuse-test

test: src/test.cpp src/file_node.cpp
	g++ src/test.cpp src/file_node.cpp -std=c++14 -g -lpthread -lopendht -lgnutls `pkg-config fuse --cflags --libs` -lstdc++ -o src/test

clean:
	rm data/main
