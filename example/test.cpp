#include <opendht.h>
#include <vector>
#include <thread>

int main() {
	dht::DhtRunner node;
	node.run(4222, dht::crypto::generateIdentity(), true);

	node.bootstrap("bootstrap.jami.net", "4222");

	bool finished = false;

    node.get(dht::InfoHash::get("123"), [](const std::vector<std::shared_ptr<dht::Value>>& values) {
        // Callback called when values are found
        for (const auto& value : values)
            std::cout << "Found value: " << *value << std::endl;
        return true; // return false to stop the search
    }, 
    [&finished](bool success) {
        std::cout << "Get finished with " << (success ? "success" : "failure") << std::endl;
		finished = true;
    });

	while (!finished);
	node.join();
	return 0;
}
