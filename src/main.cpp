#include "nbfs.h"

extern "C" {
static struct fuse_operations nbfs_ops;

void test() {
	bool finished = false;
	log_msg("test before mount");
	nbfs::node.get(
		// currently, only support the root directory
		nbfs::user_name,
		[](const std::vector<std::shared_ptr<dht::Value>>& values) {
			// fill the buf with the file name
			for (auto & val : values) {
				auto data = (*val).data.data();
				log_msg("get value: %s", (char*)data);
			}	

			// once the values are found, stop the searching
			return true;
		},
		[&finished](bool success) {
			finished = true;
		}
	);

	while (!finished); 	
}

int main(int argc, char **argv) {
	nbfs_ops.getattr = nbfs_getattr;
	nbfs_ops.readdir = nbfs_readdir;
	nbfs_ops.read	 = nbfs_read;

	open_log();

    char *d = (char*)malloc(100);
	memset(d, 0, 100);
    time_t t = time(NULL);
    struct tm *tmp = localtime(&t);
    strftime(d, 99, "%c", tmp);

    // Launch a dht node on a new thread, using a
    // generated RSA key pair, and listen on port 4222.
    nbfs::node.run(4222, dht::crypto::generateIdentity(), true);

    // Join the network through any running node,
    // here using a known bootstrap node.
    nbfs::node.bootstrap("bootstrap.jami.net", "4222");

	test();
	log_msg("BEGIN LOG nbfs-mount %s", d);
	return fuse_main(argc, argv, &nbfs_ops, NULL);
}
}
