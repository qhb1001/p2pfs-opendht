#include "nbfs.h"

#include <stdarg.h>

FILE* logfile;

void open_log() {
  logfile = fopen("rs-fuse.log", "a+");

  if(!logfile) {
    perror("Failed to open log");
    abort();
  }

  //logfile = stderr; 
}

void log_msg(const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  vfprintf(logfile, format, ap);
  va_end(ap);
  fprintf(logfile, "\n");
  fflush(logfile);
}

std::string strip_leading_slash(std::string filename) {
	bool starts_with_slash = false;
	
	if( filename.size() > 0 )
		if( filename[0] == '/' )
			starts_with_slash = true;
		
	return starts_with_slash ? filename.substr(1, filename.length() - 1) : filename;
}

