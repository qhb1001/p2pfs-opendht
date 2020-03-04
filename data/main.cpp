#include <opendht.h>
#include <string>
#include <vector>

using namespace std;

string user_name = "hongbo/";
dht::DhtRunner node;

void verifyValue(string key) {
	// search for the file
	bool finished = false;

	node.get(
		key,
		[key](const std::vector<std::shared_ptr<dht::Value>>& values) {
			// compute the size of the file
			for (const auto& value : values) {
				std::cout << "Found value: " << *value << std::endl;
				std::cout << "	with key: " << key << ", value: " << (*value).data.data() << std::endl;
				
			}
			// once the target is found, stop searching
			return true;
		},
		[&finished](bool success) {
			// the order matters
			finished = true;
		}
	);

	// spin lock to wair for searching result
	while (!finished);	
}

void prepareDirectory() {
	dht::InfoHash key = dht::InfoHash::get(user_name);
	string email = "email";
	vector<uint8_t> email_value(email.begin(), email.end());

	string name = "name";
	vector<uint8_t> name_value(name.begin(), name.end());

	string desc = "desc";
	vector<uint8_t> desc_value(desc.begin(), desc.end());

	node.put(key, email_value);
	node.put(key, name_value);
	node.put(key, desc_value);

	verifyValue(user_name);
}

int main() {
	node.run(4222, dht::crypto::generateIdentity(), true);

	node.bootstrap("bootstrap.jami.net", "4222");

	prepareDirectory();

	// data preparation
	std::string name_string = "hongbo qin\n";
	std::vector<uint8_t> name(name_string.begin(), name_string.end());

	std::string email_string = "hongbo.qin.1001@gmail.com\n";
	std::vector<uint8_t> email(email_string.begin(), email_string.end());

	std::string desc_string = "hello world!\nI am hungry!\n";
	std::vector<uint8_t> desc(desc_string.begin(), desc_string.end());
	
	// fill the key-value pair into the system
	node.put("hongbo/name", name);
	node.put("hongbo/email", email);
	node.put("hongbo/desc", desc);

	verifyValue("hongbo/name");
	verifyValue("hongbo/email");
	verifyValue("hongbo/desc");

	return 0;
}
