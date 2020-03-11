#include <gtest/gtest.h>
#include "getattr.h"
/*
TEST(P2PNodeTest, init){
    EXPECT_EQ(, testInit());
}
*/
TEST(P2PNodeTest, apply){
    EXPECT_EQ(true, testOFF_T());
}

int main(int argc, char **argv) {
    std::string foo = "hello world";
    std::vector<uint8_t> bar (foo.begin(), foo.end());
    testInit();
    
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
