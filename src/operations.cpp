/*
 *  This program is designed to mount a file system on the p2p file network supported by OpenDHT.
 *  This is the first phrase of the complex file system. And the first goal therefore is small.
 *  The file system will 
 *  (1) support read operations from the hard-coded user namespace;
 *  (2) read the content of the only one directory, which is the root directory;
 */ 

#include "nbfs.h"

extern "C" {
int nbfs_getattr(const char* path, struct stat* stbuf) {
	std::string filename = path;
	std::string clean_path = strip_leading_slash(filename);

	// return value
	int res = 0;

	
	// initialize stat
	memset(stbuf, 0, sizeof(struct stat));
	stbuf->st_uid = getuid();
	stbuf->st_gid = getgid();
	stbuf->st_atime = stbuf->st_mtime = stbuf->st_ctime = time(NULL);

	if (filename == "/") {
		log_msg("OP GETATTR(%s): Returning attributes", clean_path.c_str());
		// access mode, 0777 means that masks the RWX for group, user and other, see chmod(2)
		stbuf->st_mode = S_IFDIR | 0777;
		stbuf->st_nlink = 2;
	} else if (exists[clean_path]) {
		stbuf->st_mode = S_IFREG | 0777;
		stbuf->st_nlink = 1;
		FileNode* node = root->get(clean_path);
		log_msg("OP GETATTR(%s): Returning attributes, content[%d] of size[%d]", clean_path.c_str(), node->getContent(), strlen(node->getContent()));
		stbuf->st_size = strlen(node->getContent());
	} else {
		log_msg("OP GETATTR(%s) not found", clean_path.c_str());
		res = -ENOENT;
	}
	
	return res;
}

// read the directory content and fill the each file / directory entry to buf by filler
int nbfs_readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi) {
	log_msg("READDIR GOT PATH: %s", path);
	
	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	
	std::vector<FileNode*> directory;
	// currently only support root directory
	if (strcmp(path, "/") == 0) {
		log_msg("the root directory got %d entries", root->getSubFiles().size());
		directory = root->getSubFiles();
	} else {
		
	}

	// fill the buf
	for (auto node : directory) {
		filler(buf, node->getName().c_str(), NULL, 0);
	}

	return 0;
}

int nbfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	// initialization
	log_msg("OP READ %s", path);
	bool finished = false;
	bool find = false;
	std::string clean_path = strip_leading_slash(path);
	
	FileNode* node = root->get(strip_leading_slash(path));
	if (node == NULL) {
		log_msg("file [%s] doesn't exist", path);
		return -ENOENT;
	}

	std::string content = node->getContent();
	int length = content.length();
	log_msg("Get the content from the file %s of %d bytes", clean_path.c_str(), length);
	
	memcpy(buf, content.c_str(), length);

	log_msg("OP READ: Returning");

	return size;
}
}
int nbfs_open(const char *path, struct fuse_file_info *fi) {
	std::string clean_path = strip_leading_slash(path);
	FileNode* node = root->get(clean_path);

	int result = node? 0 : -ENOENT;
	return result;
}
