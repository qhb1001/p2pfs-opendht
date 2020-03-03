#include <vector>
#include <cstring>
#include <iostream>

int main() {
	std::vector<unsigned char> test = {65, 66, 67};
	std::cout << test.data() << std::endl;
	std::cout << "length of the string: " << std::strlen((char*)test.data()) << std::endl;
}
