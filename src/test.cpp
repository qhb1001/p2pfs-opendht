#include "nbfs.h"

int main() {
	std::string input;

	FileNode* root = new FileNode("");
	FileNode* node1 = new FileNode("foo");
	FileNode* node2 = new FileNode("bar");

	root->insert(node1); root->insert(node2);

	std::vector<FileNode*> files = root->getSubFiles();

	for (auto node : files) {
		std::cout << "file [" << node->getName() << "]\n";
	}

	
	

}
