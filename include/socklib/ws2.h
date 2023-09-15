#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>

namespace sock {
class WS2Lib {
 public:
  using Self = WS2Lib;

 public:
  WS2Lib();
  WS2Lib(const Self&) = delete;
  Self& operator=(const Self&) = delete;
  WS2Lib(Self&&);
  Self& operator=(Self&&);
  ~WS2Lib();

 private:
  enum class Status {
    Invalid,
    Valid,
  };

 private:
  WSADATA wsa_data_;
  Status status_;
};
}  // namespace sock
