#include "s21/socket.h"

#include <system_error>

struct WS2Lib {
 public:
  WS2Lib() {
    WSADATA wsa_data{0};
    int result = WSAStartup(WINSOCK_VERSION, &wsa_data);
    if (result != 0)
      throw std::system_error(WSAGetLastError(), std::system_category());
  }

  WS2Lib(const WS2Lib&) = delete;

  WS2Lib(WS2Lib&&) = delete;

  ~WS2Lib() { WSACleanup(); }

  WS2Lib& operator=(const WS2Lib&) = delete;

  WS2Lib& operator=(WS2Lib&&) = delete;
};

static const WS2Lib WS2_;

s21::SockAddr::SockAddr() {}
s21::SockAddr::SockAddr(s21::family f, std::string_view addr, uint16_t port)
    : family_(f), addr_(addr), port_(port) {}

struct sockaddr_in s21::SockAddr::sockaddr_in() const {
  struct sockaddr_in service = {0};
  service.sin_family = AF_INET;
  service.sin_port = htons(port_);
  inet_pton(family_, addr_.data(), &service.sin_addr.s_addr);
  return service;
}

s21::SockAddr::~SockAddr() {}

s21::socket::socket() {}

s21::socket::socket(s21::family f, type t, protocol proto)
    : sd_(::socket(f, t, proto)) {}

s21::socket::socket(socket&& other) noexcept
    : sd_(std::exchange(other.sd_, INVALID_SOCKET)) {}

s21::socket::~socket() {
  if (connected_) shutdown();
  closesocket(sd_);
}

s21::socket& s21::socket::operator=(socket&& rhs) noexcept {
  sd_ = std::exchange(rhs.sd_, INVALID_SOCKET);
  return *this;
}

void s21::socket::bind(SockAddr addr) {
  auto service = addr.sockaddr_in();
  int res = ::bind(sd_, (const sockaddr*)&service, sizeof(service));
  if (res == SOCKET_ERROR)
    throw std::system_error(WSAGetLastError(), std::system_category());
}

void s21::socket::connect(SockAddr addr) {
  auto service = addr.sockaddr_in();
  int res = ::connect(sd_, (const sockaddr*)&service, sizeof(service));
  if (res == SOCKET_ERROR)
    throw std::system_error(WSAGetLastError(), std::system_category());
}

void s21::socket::listen(int backlog) {
  int res =
      ::listen(sd_, (backlog == SOMAXCONN ? backlog : SOMAXCONN_HINT(backlog)));
  if (res == SOCKET_ERROR)
    throw std::system_error(WSAGetLastError(), std::system_category());
}

s21::socket s21::socket::accept() {
  SOCKET client = ::accept(sd_, nullptr, nullptr);
  if (client == SOCKET_ERROR)
    throw std::system_error(WSAGetLastError(), std::system_category());
  return socket(client);
}

std::vector<uint8_t> s21::socket::recieve(size_t bufsz) {
  std::vector<uint8_t> buf(bufsz);
  int bytes = ::recv(sd_, (char*)buf.data(), (int)buf.size(), 0);
  if (bytes == -1)
    throw std::system_error(WSAGetLastError(), std::system_category());
  buf.resize(bytes);
  connected_ = true;
  return buf;
}

int s21::socket::send(std::string_view buf) {
  connected_ = true;
  return ::send(sd_, buf.data(), (int)buf.size(), 0);
}

void s21::socket::shutdown() {
  int result = ::shutdown(sd_, SD_BOTH);
  if (result == SOCKET_ERROR)
    throw std::system_error(WSAGetLastError(), std::system_category());
}

bool s21::socket::is_valid() const { return sd_ != INVALID_SOCKET; }

s21::socket::socket(SOCKET raw) : sd_(raw) {}
