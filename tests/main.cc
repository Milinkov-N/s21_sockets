#include <gtest/gtest.h>

#include <string>
#include <thread>

#include "socklib/socklib.h"

using sock::SockAddr;

TEST(Socket, Constructor) {
  sock::WS2Lib ws2;
  sock::Socket sock(sock::Family::Ipv4, sock::Type::Stream);

  ASSERT_EQ(sock.is_valid(), true);
}

TEST(Socket, Bind) {
  sock::WS2Lib ws2;
  sock::Socket sock(sock::Family::Ipv4, sock::Type::Stream);

  sock.bind(SockAddr(sock::Family::Ipv4, "127.0.0.1", 3000));

  ASSERT_EQ(sock.is_valid(), true);
}

TEST(Socket, ListenAndConnect) {
  sock::WS2Lib ws2;
  sock::Socket listener(sock::Family::Ipv4, sock::Type::Stream);

  listener.bind(SockAddr(sock::Family::Ipv4, "127.0.0.1", 3000));
  listener.listen();

  std::jthread([]() {
    sock::Socket client(sock::Family::Ipv4, sock::Type::Stream);

    client.connect(SockAddr(sock::Family::Ipv4, "127.0.0.1", 3000));
  }).join();
}

TEST(Socket, Accept) {
  sock::WS2Lib ws2;
  sock::Socket listener(sock::Family::Ipv4, sock::Type::Stream);

  listener.bind(SockAddr(sock::Family::Ipv4, "127.0.0.1", 3000));
  listener.listen();

  std::jthread([]() {
    sock::Socket client(sock::Family::Ipv4, sock::Type::Stream);

    client.connect(SockAddr(sock::Family::Ipv4, "127.0.0.1", 3000));
  }).join();

  auto conn = listener.accept();
}

TEST(Socket, RecieveAndSend) {
  sock::WS2Lib ws2;
  sock::Socket listener(sock::Family::Ipv4, sock::Type::Stream);
  constexpr auto payload = "Some bytes";

  listener.bind(SockAddr(sock::Family::Ipv4, "127.0.0.1", 3000));
  listener.listen();

  std::jthread([]() {
    sock::Socket client(sock::Family::Ipv4, sock::Type::Stream);

    client.connect(SockAddr(sock::Family::Ipv4, "127.0.0.1", 3000));
    client.send(payload);
  }).join();

  auto conn = listener.accept();

  auto bytes = conn.recieve(16);
  std::string str(bytes.begin(), bytes.end());

  ASSERT_EQ(str, payload);
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
