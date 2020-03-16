#include "nbfs.h"


void initializeFileNodes(FileNode* root, std::string path) {
	bfs(root, path);

	std::vector<FileNode*> files = root->getSubFiles();

	log_msg("test root directory content");
	for (auto node : files) {
		log_msg("  find [%s] file", node->getName().c_str());
	}
}

void initializeOpenDHT() {
    // Launch a dht node on a new thread, using a
    // generated RSA key pair, and listen on port 4222.
    node.run(4222, dht::crypto::generateIdentity(), true);

    // Join the network through any running node,
    // here using a known bootstrap node.
    node.bootstrap("bootstrap.jami.net", "4222");
}

void initializeLog() {
	open_log();

    char *d = (char*)malloc(100);
	memset(d, 0, 100);
    time_t t = time(NULL);
    struct tm *tmp = localtime(&t);
    strftime(d, 99, "%c", tmp);

	log_msg("BEGIN LOG nbfs-mount %s", d);
}