#include <map>
#include <opendht.h>
#include <string>
#include <iostream>

extern "C" {

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
	std::string foo = "hello world";
	std::vector<uint8_t> bar (foo.begin(), foo.end());


}

}
