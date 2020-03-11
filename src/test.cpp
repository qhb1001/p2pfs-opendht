#include "nbfs.h"

int main() {
	std::string input;

	while (true) {
	
	std::cin >> input;
	FileNode x = FileNode(input);

	for (auto val : x.parse(input)) std::cout << val << std::endl;
	}
}
