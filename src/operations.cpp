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
	std::string stripped_slash = strip_leading_slash(filename);

	// return value
	int res = 0;
	// file size
	off_t filesize;

	
	// initialize stat
	memset(stbuf, 0, sizeof(struct stat));
	stbuf->st_uid = getuid();
	stbuf->st_gid = getgid();
	stbuf->st_atime = stbuf->st_mtime = stbuf->st_ctime = time(NULL);

	if (filename == "/") {
		log_msg("OP GETATTR(%s): Returning attributes", filename.c_str());
		// access mode, 0777 means that masks the RWX for group, user and other, see chmod(2)
		stbuf->st_mode = S_IFDIR | 0777;
		stbuf->st_nlink = 2;
	} else if (file_exists(stripped_slash, filesize)) {
		log_msg("OP GETATTR(%s): Returning attributes", stripped_slash.c_str());
		stbuf->st_mode = S_IFREG | 0777;
		stbuf->st_nlink = 1;
		stbuf->st_size = filesize;
	} else {
		log_msg("OP GETATTR(%s) not found", stripped_slash.c_str());
		res = -ENOENT;
	}
	
	return res;
}

// read the directory content and fill the each file / directory entry to buf by filler
int nbfs_readdir(const char* path, void* buf, fuse_fill_dir_t filler,
							off_t offset, struct fuse_file_info* fi) {
	// currently, nbfs only supports one directory, which is the root
	if (strcmp(path, "/") != 0) {
		log_msg("OP READDIR %s: Only / allowed", path);
		return -ENOENT;
	}

	log_msg("READDIR GOT PATH: %s", "/");
	
	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);

	bool finished = false;
	log_msg("READDIR begins to get the values under key %s", nbfs::user_name.c_str());
	nbfs::node.get(
		// currently, only support the root directory
		nbfs::user_name,
		[filler, &buf](const std::vector<std::shared_ptr<dht::Value>>& values) {
			// fill the buf with the file name
			for (auto & val : values) {
				auto data = (*val).data.data();
				filler(buf, (char*)data, NULL, 0);
				log_msg("READDIR %s GOT ENTRY %s", "/", (char*)data);
			}	

			// once the values are found, stop the searching
			return true;
		},
		[&finished](bool success) {
			log_msg("READDIR finished");
			finished = true;
		}
	);

	while (!finished); 
	return 0;
}

int nbfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	// initialization
	log_msg("OP READ %s", path);
	bool finished = false;
	bool find = false;
	std::string content = "";
	std::string clean_path = nbfs::user_name + path;

	// get the content
	nbfs::node.get(
		clean_path,
		[&content](const std::vector<std::shared_ptr<dht::Value>>& values) {
			for (auto& val : values) {
				auto data = (*val).data.data();
				log_msg("Found value: %s", data);
				content += (char*)data;
			}

			// stop searching once the values are found
			return true;
		},
		[&finished, &find](bool success) {
			find = success;
			log_msg("OP READ: Returning");
			finished = true;
		}
	);
	
	while (!finished);
	if (!find) return -ENOENT;

	off_t length = content.length();
	if (offset + size > length) size = length - offset;

	log_msg("Get the content from the file %s of %d bytes", clean_path, size);
	
	memcpy(buf, content.c_str() + offset, size);

	log_msg("OP READ: Returning");

	return size;
}
}
