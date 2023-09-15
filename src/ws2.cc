#include "socklib/ws2.h"

#include <system_error>

sock::WS2Lib::WS2Lib() {
  int result = WSAStartup(WINSOCK_VERSION, &wsa_data_);
  if (result != 0)
    throw std::system_error(WSAGetLastError(), std::system_category());
  status_ = Status::Valid;
}

sock::WS2Lib::WS2Lib(Self&& other)
    : wsa_data_(other.wsa_data_),
      status_(std::exchange(other.status_, Status::Invalid)) {}

sock::WS2Lib& sock::WS2Lib::operator=(Self&& rhs) {
  wsa_data_ = rhs.wsa_data_;
  status_ = std::exchange(rhs.status_, Status::Invalid);
  return *this;
}

sock::WS2Lib::~WS2Lib() {
  if (status_ == Status::Valid) WSACleanup();
}