#include "socklib/socklib.h"

#include <system_error>

sock::Winsock2Lib::Winsock2Lib() {
  int result = WSAStartup(WINSOCK_VERSION, &wsa_data_);
  if (result != 0)
    throw std::system_error(WSAGetLastError(), std::system_category());
}

sock::Winsock2Lib::~Winsock2Lib() { WSACleanup(); }

sock::Socket::Socket() {}

sock::Socket::~Socket() {}
