#include "nbfs.h"

static struct fuse_operations nbfs_ops = {
	.getattr = nbfs_getattr,
	.readdir = nbfs_readdir,
	.read = nbfs_read,
};

int main(int argc, char **argv) {
	open_log();

    char *d = (char*)malloc(100);
	memset(d, 0, 100);
    time_t t = time(NULL);
    struct tm *tmp = localtime(&t);
    strftime(d, 99, "%c", tmp);

	log_msg("BEGIN LOG nbfs-mount %s", d);
	return fuse_main(argc, argv, &nbfs_ops, NULL);
}
