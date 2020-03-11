#include "nbfs.h"

extern "C" {
static struct fuse_operations nbfs_ops;

void fillNodes(FileNode* node, std::string key) {
	bool finished = false;
	nbfs::node.get(
		nbfs::user_name + key,
		[&node, key](const std::vector<std::shared_ptr<dht::Value>>& values) {
			for (auto & val : values) {
				log_msg("  -> filling the content of regular file [%s]", key.c_str());
				char* content = (char*)((*val).data.data());
				node->setContent(content);
			}
			return false;
		},
		[&finished](bool success) {
			finished = true;
		}
	);

	while (!finished);
}

void initializeFileNodes(FileNode* curNode, std::string absolutePath) {
	bool finished = false;
	nbfs::node.get(
		nbfs::user_name + absolutePath,
		[&curNode, absolutePath](const std::vector<std::shared_ptr<dht::Value>>& values) {
			for (auto & val : values) {
				char* name = (char*)((*val).data.data());
				int length = strlen(name);
				FileNode* newNode = new FileNode(name);

				// if this is a directory
				if (name[length - 1] == '/') {
					log_msg("%s: a directory [%s] is found", absolutePath.c_str(), name);
					curNode->insert(newNode);
					initializeFileNodes(newNode, absolutePath + name);
				} else {
					log_msg("%s: a regular file [%s] is found", absolutePath.c_str(), name);
					fillNodes(newNode, absolutePath + name);
					curNode->insert(newNode);
					// terminate
				}
			}	

			// once the values are found, stop the searching
			return false;
		},
		[&finished](bool success) {
			finished = true;
		}
	);

	while (!finished); 	
}

void initializeOpenDHT() {
    // Launch a dht node on a new thread, using a
    // generated RSA key pair, and listen on port 4222.
    nbfs::node.run(4222, dht::crypto::generateIdentity(), true);

    // Join the network through any running node,
    // here using a known bootstrap node.
    nbfs::node.bootstrap("bootstrap.jami.net", "4222");
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

int main(int argc, char **argv) {
	initializeOpenDHT();

	initializeLog();

	nbfs::root = new FileNode("");
	initializeFileNodes(nbfs::root, "");

	nbfs_ops.getattr = nbfs_getattr;
	nbfs_ops.readdir = nbfs_readdir;
	nbfs_ops.read	 = nbfs_read;

	return fuse_main(argc, argv, &nbfs_ops, NULL);
}
}
