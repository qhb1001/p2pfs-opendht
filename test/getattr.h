#include "init.h"

bool testOFF_T() {
    off_t filesize = 0;
    int sum = 0;
    // search for the file
    bool finished = false;
    bool find = false;
    node.get(
             "hongbo",
             [&filesize, &sum](const std::vector<std::shared_ptr<dht::Value>>& values) {
                 // compute the size of the file
                 for (const auto& value : values) {
                     std::cout << "Found value: " << *value << std::endl;
                     auto data = (*value).data.data();
                     sum += strlen((char*)data);
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
    return sum == filesize;
}
