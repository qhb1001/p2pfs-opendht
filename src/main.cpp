#include "nbfs.h"

dht::DhtRunner node;

FileNode* root;

std::map<std::string, bool> exists;

extern "C" {
static struct fuse_operations nbfs_ops;


int parseValue(char* val) {
	int length = strlen(val);
	int i = 0, result = 0;
	for (; i < length; i++) {
		if (val[i] == '[') {
			i += 1;
			break;
		}
	}

	for (; i < length; i++) {
		if (val[i] == ']') break;
		result = result * 10 + val[i] - '0';
	}

	return result;
}

void fillNodes(FileNode* fileNode, std::string key) {
	log_msg("  -> filling the content of regular file [%s]", key.c_str());
	bool finished = false;
	node.get(
		user_name + key,
		[&fileNode, key](const std::vector<std::shared_ptr<dht::Value>>& values) {
			for (auto & val : values) {
				char* content = (char*)((*val).data.data());
				content[parseValue(content)] = '\0';
				fileNode->setContent(content);
				log_msg("      got the content [%s]", content);
			}
			return false;
		},
		[&finished](bool success) {
			finished = true;
		}
	);

	while (!finished);
}

// dfs method to fetch the content from OpenDHT [deprecated]
void dfs(FileNode* curNode, std::string absolutePath) {
	bool finished = false;
	node.get(
		user_name + absolutePath,
		[&curNode, absolutePath](const std::vector<std::shared_ptr<dht::Value>>& values) {
			for (auto & val : values) {
				char* name = (char*)((*val).data.data());
				int length = parseValue(name);
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
			return true;
		},
		[&finished](bool success) {
			finished = true;
		}
	);

	while (!finished); 	
}

void bfs(FileNode* root, std::string absolutePath) {
	std::queue<std::pair<FileNode*, std::string>> q;
	FileNode* curNode;
	std::string curPath;
	q.push(std::make_pair(root, ""));

	while (!q.empty()) {
		// get the state of the current file
		auto filePair = q.front();
		q.pop();
		curNode = filePair.first;
		curPath = filePair.second;

		// start to request the content
		bool finished = false;
		std::pair<FileNode*, std::string> regularFile;
		regularFile.first = NULL;

		node.get(
			user_name + curPath,
			[&curNode, curPath, &regularFile, &q](const std::vector<std::shared_ptr<dht::Value>>& values) {
				for (auto & val : values) {
					char* name = (char*)((*val).data.data());
					int length = parseValue(name);
					name[length] = '\0';
					if (exists[curPath + name] == true) continue;
					else exists[curPath + name] = true;

					FileNode* newNode = new FileNode(name);
					for (int i = 0; i < length; i++) {
						log_msg("The %d-th char [%c]", i, name[i]);
					}

					// if this is a directory
					if (name[length - 1] == '/') {
						log_msg("%s: a directory [%s] is found", curPath.c_str(), name);
						curNode->insert(newNode);
						q.push(std::make_pair(newNode, curPath + name));
					} else {
						log_msg("%s: a regular file [%s] is found", curPath.c_str(), name);
						regularFile = std::make_pair(newNode, curPath + name);
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

		// if this is a regular file, try to fill it
		if (regularFile.first != NULL) {
			fillNodes(regularFile.first, regularFile.second);
		}
	}
}

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

int main(int argc, char **argv) {
	initializeOpenDHT();

	initializeLog();

	root = new FileNode("");
	initializeFileNodes(root, "");

	nbfs_ops.getattr = nbfs_getattr;
	nbfs_ops.readdir = nbfs_readdir;
	nbfs_ops.read	 = nbfs_read;

	return fuse_main(argc, argv, &nbfs_ops, NULL);
}
}
