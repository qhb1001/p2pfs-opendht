.PHONY: all test clean data
data: data/main.cpp
	g++ data/main.cpp -std=c++14 -lopendht -lgnutls -o data/main
	./data/main

test_getattr: test/test_getattr.cpp
	g++ test/test_getattr.cpp -std=c++14 -lopendht -lgnutls `pkg-config fuse --cflags --libs` -lstdc++ -o test/test_getattr

clean:
	rm data/main
