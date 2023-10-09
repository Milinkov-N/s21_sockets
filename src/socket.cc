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
s21::SockAddr::SockAddr(Family family, std::string_view addr, uint16_t port)
    : family_(family), addr_(addr), port_(port) {}

struct sockaddr_in s21::SockAddr::sockaddr_in() const {
  struct sockaddr_in service = {0};
  service.sin_family = AF_INET;
  service.sin_port = htons(port_);
  inet_pton((int)family_, addr_.data(), &service.sin_addr.s_addr);
  return service;
}

s21::SockAddr::~SockAddr() {}

s21::Socket::Socket() {}

s21::Socket::Socket(Family family, Type type, Proto proto)
    : sd_(socket((int)family, (int)type, (int)proto)) {}

s21::Socket::Socket(Socket&& other) noexcept
    : sd_(std::exchange(other.sd_, INVALID_SOCKET)) {}

s21::Socket::~Socket() {
  if (connected_) shutdown();
  closesocket(sd_);
}

s21::Socket& s21::Socket::operator=(Socket&& rhs) noexcept {
  sd_ = std::exchange(rhs.sd_, INVALID_SOCKET);
  return *this;
}

void s21::Socket::bind(SockAddr addr) {
  auto service = addr.sockaddr_in();
  int res = ::bind(sd_, (const sockaddr*)&service, sizeof(service));
  if (res == SOCKET_ERROR)
    throw std::system_error(WSAGetLastError(), std::system_category());
}

void s21::Socket::connect(SockAddr addr) {
  auto service = addr.sockaddr_in();
  int res = ::connect(sd_, (const sockaddr*)&service, sizeof(service));
  if (res == SOCKET_ERROR)
    throw std::system_error(WSAGetLastError(), std::system_category());
}

void s21::Socket::listen(int backlog) {
  int res =
      ::listen(sd_, (backlog == SOMAXCONN ? backlog : SOMAXCONN_HINT(backlog)));
  if (res == SOCKET_ERROR)
    throw std::system_error(WSAGetLastError(), std::system_category());
}

s21::Socket s21::Socket::accept() {
  SOCKET client = ::accept(sd_, nullptr, nullptr);
  if (client == SOCKET_ERROR)
    throw std::system_error(WSAGetLastError(), std::system_category());
  return Socket(client);
}

std::vector<uint8_t> s21::Socket::recieve(size_t bufsz) {
  std::vector<uint8_t> buf(bufsz);
  int bytes = ::recv(sd_, (char*)buf.data(), (int)buf.size(), 0);
  if (bytes == -1)
    throw std::system_error(WSAGetLastError(), std::system_category());
  buf.resize(bytes);
  connected_ = true;
  return buf;
}

int s21::Socket::send(std::string_view buf) {
  connected_ = true;
  return ::send(sd_, buf.data(), (int)buf.size(), 0);
}

void s21::Socket::shutdown() {
  int result = ::shutdown(sd_, SD_BOTH);
  if (result == SOCKET_ERROR)
    throw std::system_error(WSAGetLastError(), std::system_category());
}

bool s21::Socket::is_valid() const { return sd_ != INVALID_SOCKET; }

s21::Socket::Socket(SOCKET raw) : sd_(raw) {}
