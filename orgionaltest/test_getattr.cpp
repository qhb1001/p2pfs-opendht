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

dht::DhtRunner node;
void testInit() {
	node.run(4222, dht::crypto::generateIdentity(), true);
	node.bootstrap("bootstrap.jami.net", "4222");
}

bool testOFF_T() {
	off_t filesize = 0;	
	int sum = 0;
	// search for the file
	bool finished = false;
	bool find = false;
	node.get(
		"hongbo",
		[&filesize, &sum](const std::vector<std::shared_ptr<dht::Value>>& values) {
			// compute the size of the file
			for (const auto& value : values) {
				std::cout << "Found value: " << *value << std::endl;
				auto data = (*value).data.data();
				sum += strlen((char*)data);
				filesize += strlen((char*)data);
			}
			// once the target is found, stop searching
			return false;
		},
		[&finished, &find](bool success) {
			// the order matters
			find = success;
			finished = true;
		}
	);

	// spin lock to wair for searching result
	while (!finished);
	
	return sum == filesize;
}

int main() {
	std::string foo = "hello world";
	std::vector<uint8_t> bar (foo.begin(), foo.end());
	
	testInit();
	std::cout << (testOFF_T()? "." : "x") << std::endl;
	return 0;
}

}
