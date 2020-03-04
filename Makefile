.PHONY: all test clean data
data: data/main.cpp
	g++ data/main.cpp -std=c++14 -lopendht -lgnutls -o data/main
	./data/main

clean:
	rm data/main
