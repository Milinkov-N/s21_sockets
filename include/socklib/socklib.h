#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>

namespace sock {
class Winsock2Lib {
 public:
  using Self = Winsock2Lib;

 public:
  Winsock2Lib();
  ~Winsock2Lib();

  Winsock2Lib(const Self&) = delete;
  Self& operator=(const Self&) = delete;
  Winsock2Lib(const Self&&) = delete;
  Self& operator=(const Self&&) = delete;

 private:
  WSADATA wsa_data_;
};

class Socket {
 public:
  Socket();
  ~Socket();

 private:
};
}  // namespace sock
