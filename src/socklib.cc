#include "socklib/socklib.h"

sock::SockAddr::SockAddr() {}
sock::SockAddr::SockAddr(AddrFamily family, std::string_view addr,
                         uint16_t port)
    : family_(family), addr_(addr), port_(port) {}

struct sockaddr_in sock::SockAddr::sockaddr_in() const {
  struct sockaddr_in service = {0};
  service.sin_family = AF_INET;
  service.sin_port = htons(port_);
  inet_pton((int)family_, addr_.data(), &service.sin_addr.s_addr);
  return service;
}

sock::SockAddr::~SockAddr() {}

sock::Socket::Socket() {}

sock::Socket::Socket(SockAddr sock_addr) : sock_addr_(sock_addr) {}

sock::Socket::~Socket() {}
