#include "nbfs.h"

#include <stdarg.h>

FILE *logfile;

void open_log() {
  logfile = fopen("rs-fuse.log", "a+");

  if(! logfile) {
    perror("Failed to open log");
    abort();
  }

  //logfile = stderr; 
}

void log_msg(char *format, ...) {
  va_list ap;
  va_start(ap, format);
  vfprintf(logfile, format, ap);
  va_end(ap);
  fprintf(logfile, "\n");
  fflush(logfile);
}

bool file_exists(std::string fileName, off_t& filesize) {
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


std::string strip_leading_slash(std::string filename) {
	bool starts_with_slash = false;
	
	if( filename.size() > 0 )
		if( filename[0] == '/' )
			starts_with_slash = true;
		
	return starts_with_slash ? filename.substr(1, filename.length() - 1) : filename;
}

