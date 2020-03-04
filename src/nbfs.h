#ifndef _NBFS_H
#define _NBFS_H

#include <string>
#include <iostream>
#include <vector>
#include <opendht.h>

#define FUSE_USE_VERSION  26
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

// hard coded user name, used for the namespace for files
namespace nbfs {
	static std::string user_name = "hongbo/";
}

// connection entry
dht::DhtRunner node;

/* FUSE operations (operations.cpp) */
int nbfs_getattr(const char *path, struct stat *stbuf);
int nbfs_open(const char *path, struct fuse_file_info *fi);
int nbfs_read(const char *path, char *buf, size_t size, off_t offset,
            struct fuse_file_info *fi);
int nbfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset,
               struct fuse_file_info *fi);

/* helpers (helpers.cpp) */

/* 
 * check whether or not the current file exists; if the file exists, the function
 * will fill the filesize field. 
 * @ fileName: name of the file
 * @ filesize: the size of the file in bytes. 
 */ 
bool file_exists(std::string fileName, off_t& filesize);
std::string strip_leading_slash(std::string filename);
void open_log();
void log_msg(char *format, ...);

#endif
