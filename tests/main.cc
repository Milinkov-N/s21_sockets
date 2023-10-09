#include <gtest/gtest.h>

#include <string>
#include <thread>

#include "s21/socket.h"

using s21::SockAddr;

TEST(Socket, Constructor) {
  s21::WS2Lib ws2;
  s21::Socket sock(s21::Family::Ipv4, s21::Type::Stream);

  ASSERT_EQ(sock.is_valid(), true);
}

TEST(Socket, Bind) {
  s21::WS2Lib ws2;
  s21::Socket sock(s21::Family::Ipv4, s21::Type::Stream);

  sock.bind(SockAddr(s21::Family::Ipv4, "127.0.0.1", 3000));

  ASSERT_EQ(sock.is_valid(), true);
}

TEST(Socket, ListenAndConnect) {
  s21::WS2Lib ws2;
  s21::Socket listener(s21::Family::Ipv4, s21::Type::Stream);

  listener.bind(SockAddr(s21::Family::Ipv4, "127.0.0.1", 3000));
  listener.listen();

  std::jthread([]() {
    s21::Socket client(s21::Family::Ipv4, s21::Type::Stream);

    client.connect(SockAddr(s21::Family::Ipv4, "127.0.0.1", 3000));
  }).join();
}

TEST(Socket, Accept) {
  s21::WS2Lib ws2;
  s21::Socket listener(s21::Family::Ipv4, s21::Type::Stream);

  listener.bind(SockAddr(s21::Family::Ipv4, "127.0.0.1", 3000));
  listener.listen();

  std::jthread([]() {
    s21::Socket client(s21::Family::Ipv4, s21::Type::Stream);

    client.connect(SockAddr(s21::Family::Ipv4, "127.0.0.1", 3000));
  }).join();

  auto conn = listener.accept();
}

TEST(Socket, RecieveAndSend) {
  s21::WS2Lib ws2;
  s21::Socket listener(s21::Family::Ipv4, s21::Type::Stream);
  constexpr auto payload = "Some bytes";

  listener.bind(SockAddr(s21::Family::Ipv4, "127.0.0.1", 3000));
  listener.listen();

  std::jthread([]() {
    s21::Socket client(s21::Family::Ipv4, s21::Type::Stream);

    client.connect(SockAddr(s21::Family::Ipv4, "127.0.0.1", 3000));
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
