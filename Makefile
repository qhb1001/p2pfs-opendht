.PHONY : default_target

main: src/nbfs.h src/operations.cpp src/helpers.cpp src/main.cpp
	g++ src/operations.cpp src/helpers.cpp src/main.cpp -std=c++14 -g -lpthread -lopendht -lgnutls `pkg-config fuse --cflags --libs` -lstdc++ -o src/fuse-test

test: src/test.cpp src/file_node.cpp
	g++ src/test.cpp src/file_node.cpp -std=c++14 -g -lpthread -lopendht -lgnutls `pkg-config fuse --cflags --libs` -lstdc++ -o src/test
