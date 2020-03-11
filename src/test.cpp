#include "nbfs.h"

int main() {
	std::string input;

	while (true) {
	
	std::cin >> input;
	FileNode x = FileNode(input);

	for (auto val : x.getPath()) std::cout << val << std::endl;
	}
}
