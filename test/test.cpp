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
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
