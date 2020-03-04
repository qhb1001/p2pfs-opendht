#include <opendht.h>
#include <string>
#include <vector>

int main() {
	dht::DhtRunner node;

	node.run(4222, dht::crypto::generateIdentity(), true);

	node.bootstrap("bootstrap.jami.net", "4222");

	// data preparation
	std::string name_string = "hongbo qin";
	std::vector<uint8_t> name(name_string.begin(), name_string.end());

	std::string email_string = "hongbo.qin.1001@gmail.com";
	std::vector<uint8_t> email(email_string.begin(), email_string.end());

	std::string desc_string = "hello world!";
	std::vector<uint8_t> desc(desc_string.begin(), desc_string.end());
	
	// fill the key-value pair into the system
	dht::InfoHash key = dht::InfoHash::get("hongbo");
	node.put("hongbo", name);
	node.put(key, email);
	node.put(key, desc);

	// verify the data exists
	bool finished = false;
	node.get(
		key, 
		[&finished](const std::vector<std::shared_ptr<dht::Value>>& values) {
			for (const auto& value : values)
           		std::cout << "Found value: " << *value << std::endl;

			if (values.size() == 3) {
				finished = true;
				return false;
			}

			return true; // else, keep searching
		},
		[](bool success) {
			std::cout << "Get finished with " << (success ? "success" : "failure") << std::endl;
		}
	);

	while (!finished); 
	return 0;
}
