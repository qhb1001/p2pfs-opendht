#ifndef _NBFS_H
#define _NBFS_H

#include <string>
#include <iostream>
#include <vector>
#include <queue>
#include <opendht.h>
#include <thread>
#include <utility>
#include <map>

#define FUSE_USE_VERSION  26
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>



extern "C" {
/* File storage structure */
class FileNode {
private:
	// if the file is a directory or not
	bool isDirectory;
	// the content of the file, if isDirectory=false 
	char* content;
	// the content of the directory, if isDirectory=true
	std::vector<FileNode*> subFiles;
	// the absolute path of the file
	std::string name;

public:
	// parser method
	std::vector<std::string> parse(std::string filePath);
	// constructor method
	FileNode(std::string filePath, char* cont = NULL);
	// insert a file node
	void insert(FileNode* node);
	// get the file node, return NULL if it does not exist
	FileNode* get(std::string filePath);
	// get subfiles
	std::vector<FileNode*> getSubFiles();
	// get name
	std::string getName();
	// set content
	void setContent(char* content);
	// get content
	char* getContent();	
};

/* initialization process */
void initializeOpenDHT();
void initializeLog();
void initializeFileNodes(FileNode*, std::string);

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
std::string strip_leading_slash(std::string filename);
void open_log();
void log_msg(const char *format, ...);
int parseValue(char*);

}

// hard coded user name, used for the namespace for files
static std::string user_name = "hongbo/";

// connection entry
extern dht::DhtRunner node;

// root file node
extern FileNode* root;

// map for file names
extern std::map<std::string, bool> exists;

// log file
extern FILE *logfile;
#endif
