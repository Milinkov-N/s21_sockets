#include "socklib/socklib.h"

#include <system_error>

sock::SockAddr::SockAddr() {}
sock::SockAddr::SockAddr(Family family, std::string_view addr, uint16_t port)
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

sock::Socket::Socket(Family family, Type type, Proto proto)
    : sd_(socket((int)family, (int)type, (int)proto)) {}

sock::Socket::Socket(Socket&& other) noexcept
    : sd_(std::exchange(other.sd_, INVALID_SOCKET)) {}

sock::Socket::~Socket() {
  if (connected_) shutdown();
  closesocket(sd_);
}

sock::Socket& sock::Socket::operator=(Socket&& rhs) noexcept {
  sd_ = std::exchange(rhs.sd_, INVALID_SOCKET);
  return *this;
}

void sock::Socket::bind(SockAddr addr) {
  auto service = addr.sockaddr_in();
  int res = ::bind(sd_, (const sockaddr*)&service, sizeof(service));
  if (res == SOCKET_ERROR)
    throw std::system_error(WSAGetLastError(), std::system_category());
}

void sock::Socket::connect(SockAddr addr) {
  auto service = addr.sockaddr_in();
  int res = ::connect(sd_, (const sockaddr*)&service, sizeof(service));
  if (res == SOCKET_ERROR)
    throw std::system_error(WSAGetLastError(), std::system_category());
}

void sock::Socket::listen(int backlog) {
  int res =
      ::listen(sd_, (backlog == SOMAXCONN ? backlog : SOMAXCONN_HINT(backlog)));
  if (res == SOCKET_ERROR)
    throw std::system_error(WSAGetLastError(), std::system_category());
}

sock::Socket sock::Socket::accept() {
  SOCKET client = ::accept(sd_, nullptr, nullptr);
  if (client == SOCKET_ERROR)
    throw std::system_error(WSAGetLastError(), std::system_category());
  return Socket(client);
}

std::vector<uint8_t> sock::Socket::recieve(size_t bufsz) {
  std::vector<uint8_t> buf(bufsz);
  int bytes = ::recv(sd_, (char*)buf.data(), (int)buf.size(), 0);
  if (bytes == -1)
    throw std::system_error(WSAGetLastError(), std::system_category());
  buf.resize(bytes);
  connected_ = true;
  return buf;
}

int sock::Socket::send(std::string_view buf) {
  connected_ = true;
  return ::send(sd_, buf.data(), (int)buf.size(), 0);
}

void sock::Socket::shutdown() {
  int result = ::shutdown(sd_, SD_BOTH);
  if (result == SOCKET_ERROR)
    throw std::system_error(WSAGetLastError(), std::system_category());
}

bool sock::Socket::is_valid() const { return sd_ != INVALID_SOCKET; }

sock::Socket::Socket(SOCKET raw) : sd_(raw) {}
