#include <gtest/gtest.h>

#include "socklib/socklib.h"

TEST(Test, Example) {
  sock::Winsock2Lib ws2;
  sock::Socket sock;

  ASSERT_EQ(true, true);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  RUN_ALL_TESTS();
  return 0;
}
