/*
 *  This program is designed to mount a file system on the p2p file network supported by OpenDHT.
 *  This is the first phrase of the complex file system. And the first goal therefore is small.
 *  The file system will 
 *  (1) support read operations from the hard-coded user namespace;
 *  (2) read the content of the only one directory, which is the root directory;
 */ 

#include <string>
#include <iostream>
#include <vector>
#include <opendht.h>

extern "C" {
#define FUSE_USE_VERSION  26
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

// hard coded user name, used for the namespace for files
const static std::string user_name = "hongbo";
// connection entry
dht::DhtRunner node;

// maybe should have a different signature: static void *limitfs_init(struct fuse_conn_info *conn, struct fuse_config *cfg)
static void* nbfs_init(struct fuse_conn_info *conn) {
	// launch a dht node on a new thread, using a 
	// generated RSA key pair, and listen on port 4222.
	node.run(4222, dht::crypto::generateIdentity(), true);

	// Join the network through any running node,
	// here using a known bootstrap node.
	node.bootstrap("bootstrap.jami.net", "4222");

	return NULL;
}

/***************** the type of the second parameter demands further investigation ******************/
// search for regular file
static bool file_exists(std::string fileName, off_t& filesize) {
	filesize = 0;
	// get the file name with prefix
	std::string fileNameWithPrefix = user_name + "::" + fileName;

	// search for the file
	bool finished = false;
	bool find = false;
	node.get(
		filenameWithPrefix,
		[](const std::vector<std::shared_ptr<dht::Value>>& values) {
			// compute the size of the file
			for (const auto& value : values) {
				std::cout << "Found value: " << *value << std::endl;
				auto data = (*value).data.data();
				filesize += strlen((char*)data);
			}
			// once the target is found, stop searching
			return false;
		},
		[&finished, &find](bool success) {
			// the order matters
			find = success;
			finished = true;
		}
	);

	// spin lock to wair for searching result
	while (!finished);

	return find;
}

static string strip_leading_slash(string filename) {
	bool starts_with_slash = false;
	
	if( filename.size() > 0 )
		if( filename[0] == '/' )
			starts_with_slash = true;
		
	return starts_with_slash ? filename.substr(1, string::npos) : filename;
}

static int nbfs_getattr(const char* path, struct stat* stbuf) {
	std::string filename = path;
	std::string stripped_slash = strip_leading_slash(filename);

	// return value
	int res = 0;
	// file size
	off_t filesize;

	
	// initialization of stat
	memset(stbuf, 0, sizeof(struct stat));
	stbuf->st_uid = getuid();
	stbuf->st_gid = getgid();
	stbuf->st_atime = stbuf->st_mtime = stbuf->st_ctime = time(NULL);

	if (filename == "/") {
		std::cout << "nbfs_getattr(" << filename << "): Returning attributes" << std::endl;
		
		// access mode, 0777 means that masks the RWX for group, user and other, see chmod(2)
		stbuf->st_mode = S_IFDIR | 0777;
		stbuf->st_nlink = 2;
	} else if (file_exists(stripped_slash, filesize)) {
		std::cout << "nbfs_getattr(" << stripped_slash << "): Returning attributes" << std::endl;

		stbuf->st_mode = S_IFREG | 0777;
		stbuf->st_nlink = 1;
		stbuf->st_size = filesize;
	} else {
		std::cout << "nbfs_getattr(" << stripped_slash << "): not found" << std::endl;
		res = -ENOENT;
	}
	
	return res;
}

}

