#include <gtest/gtest.h>

#include <string>
#include <thread>

#include "s21/socket.h"

TEST(Socket, Constructor) {
  s21::socket sock(s21::ipv4, s21::stream);

  ASSERT_EQ(sock.is_valid(), true);
}

TEST(Socket, Bind) {
  s21::socket sock(s21::ipv4, s21::stream);

  sock.bind("127.0.0.1", 3000);

  ASSERT_EQ(sock.is_valid(), true);
}

TEST(Socket, ListenAndConnect) {
  s21::socket listener(s21::ipv4, s21::stream);

  listener.bind("127.0.0.1", 3000);
  listener.listen();

  std::jthread([]() {
    s21::socket client(s21::ipv4, s21::stream);

    client.connect("127.0.0.1", 3000);
  }).join();
}

TEST(Socket, Accept) {
  s21::socket listener(s21::ipv4, s21::stream);

  listener.bind("127.0.0.1", 3000);
  listener.listen();

  std::jthread([]() {
    s21::socket client(s21::ipv4, s21::stream);

    client.connect("127.0.0.1", 3000);
  }).join();

  auto conn = listener.accept();
}

TEST(Socket, RecieveAndSend) {
  s21::socket listener(s21::ipv4, s21::stream);
  constexpr auto payload = "Some bytes";

  listener.bind("127.0.0.1", 3000);
  listener.listen();

  std::jthread([]() {
    s21::socket client(s21::ipv4, s21::stream);

    client.connect("127.0.0.1", 3000);
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
