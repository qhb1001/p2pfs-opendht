#include "nbfs.h"

std::vector<std::string> FileNode::parse(std::string filePath) {
	int length = filePath.length();
	std::string delimiter = "/";

	std::vector<std::string> result;
	size_t pos = 0;
	while ((pos = filePath.find(delimiter)) != std::string::npos) {
		result.push_back(filePath.substr(0, pos) + "/");
		filePath.erase(0, pos + delimiter.length());
	}

	if (filePath.length() != 0) result.push_back(filePath);

	return result;
}

FileNode::FileNode(std::string name) {
	this->name = name;
}

std::vector<FileNode*> FileNode::getSubFiles() {
	return subFiles;
}

std::string FileNode::getName() {
	return name;
}

void FileNode::insert(FileNode* node) {
	subFiles.push_back(node);
}

FileNode* FileNode::get(std::string path) {
	FileNode* curNode = this;
	for (auto p : parse(path)) {
		bool find = false;
		for (auto n : curNode->getSubFiles()) {
			if (p == n->getName()) {
				find = true;
				curNode = n;
				break;
			}
		}

		// not found
		if (!find) return NULL;
	}
	
	return curNode;
}

void FileNode::setContent(char* content) {
	this->content = content;
}

char* FileNode::getContent() {
	return this->content;
}
