#include <map>
#include <opendht.h>
#include <string>
#include <iostream>
//#include <../gtest/gtest.h>

extern "C" {
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
}

dht::DhtRunner node;
void testInit() {
    node.run(4222, dht::crypto::generateIdentity(), true);
    node.bootstrap("bootstrap.jami.net", "4222");
}
