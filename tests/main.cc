#include <gtest/gtest.h>

#include "socklib/socklib.h"

using sock::AddrFamily;
using sock::SockAddr;

TEST(Test, Example) {
  sock::WS2Lib ws2;
  sock::Socket sock(SockAddr(AddrFamily::Ipv4, "127.0.0.1", 3000));

  ASSERT_EQ(true, true);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  RUN_ALL_TESTS();
  return 0;
}
