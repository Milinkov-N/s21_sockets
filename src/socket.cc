#include "s21/socket.h"

#include <cstring>
#include <system_error>
#include <utility>

#ifdef _WIN32
static const struct WS2Lib {
 public:
  WS2Lib() {
    WSADATA wsa_data{0};
    int result = WSAStartup(WINSOCK_VERSION, &wsa_data);
    if (result != 0) throw std::system_error(errno, std::system_category());
  }

  WS2Lib(const WS2Lib&) = delete;

  WS2Lib(WS2Lib&&) = delete;

  ~WS2Lib() { WSACleanup(); }

  WS2Lib& operator=(const WS2Lib&) = delete;

  WS2Lib& operator=(WS2Lib&&) = delete;
} WS2_;
#endif

s21::socket::socket() {}

s21::socket::socket(s21::family f, type t, protocol proto)
    : sd_(::socket(f, t, proto)) {}

s21::socket::socket(socket&& other) noexcept
    : sd_(std::exchange(other.sd_, INVALID_SOCKET)) {}

s21::socket::~socket() {
  if (connected_) shutdown();

#ifdef _WIN32
  closesocket(sd_);
#else
  close(sd_);
#endif
}

s21::socket& s21::socket::operator=(socket&& rhs) noexcept {
  sd_ = std::exchange(rhs.sd_, INVALID_SOCKET);
  return *this;
}

#if STDCXX_17_OR_HIGHER
void s21::socket::bind(std::string_view name, uint16_t port, family af)
#else
void s21::socket::bind(const char* name, uint16_t port, family af)
#endif
{
#if STDCXX_17_OR_HIGHER
  auto addr_name = name.data();
#else
  if (name == nullptr)
    throw std::invalid_argument("s21::socket::bind() 'name' was null");

  auto addr_name = name;
#endif

  sockaddr_in service;

  memset(&service, 0, sizeof(sockaddr_in));

  service.sin_family = af;
  inet_pton(af, addr_name, &service.sin_addr.s_addr);
  service.sin_port = port;

  int res =
      ::bind(sd_, reinterpret_cast<const sockaddr*>(&service), sizeof(service));
  if (res == SOCKET_ERROR)
    throw std::system_error(errno, std::system_category());
}

#if STDCXX_17_OR_HIGHER
void s21::socket::connect(std::string_view name, uint16_t port, family af)
#else
void s21::socket::connect(const char* name, uint16_t port, family af)
#endif
{
#if STDCXX_17_OR_HIGHER
  auto addr_name = name.data();
#else
  if (name == nullptr)
    throw std::invalid_argument("s21::socket::connect() 'name' was null");

  auto addr_name = name;
#endif

  sockaddr_in service;

  memset(&service, 0, sizeof(sockaddr_in));

  service.sin_family = af;
  inet_pton(af, addr_name, &service.sin_addr.s_addr);
  service.sin_port = port;

  int res = ::connect(sd_, reinterpret_cast<const sockaddr*>(&service),
                      sizeof(service));
  if (res == SOCKET_ERROR)
    throw std::system_error(errno, std::system_category());
}

void s21::socket::listen(int backlog) {
#ifdef _WIN32
  int res =
      ::listen(sd_, (backlog == SOMAXCONN ? backlog : SOMAXCONN_HINT(backlog)));
#else
  int res = ::listen(sd_, backlog);
#endif
  if (res == SOCKET_ERROR)
    throw std::system_error(errno, std::system_category());
}

s21::socket s21::socket::accept() {
  SOCKET client = ::accept(sd_, nullptr, nullptr);
  if (client == SOCKET_ERROR)
    throw std::system_error(errno, std::system_category());
  return socket(client);
}

std::vector<uint8_t> s21::socket::recieve(size_t bufsz) {
  std::vector<uint8_t> buf(bufsz);
  int bytes = ::recv(sd_, (char*)buf.data(), (int)buf.size(), 0);
  if (bytes == -1) throw std::system_error(errno, std::system_category());
  buf.resize(bytes);
  connected_ = true;
  return buf;
}

#if STDCXX_17_OR_HIGHER
int s21::socket::send(std::string_view buf)
#else
int s21::socket::send(const std::vector<uint8_t>& buf)
#endif
{
  connected_ = true;
  return ::send(sd_, reinterpret_cast<const char*>(buf.data()), (int)buf.size(),
                0);
}

void s21::socket::shutdown() {
  int result = ::shutdown(sd_, SD_BOTH);
  if (result == SOCKET_ERROR)
    throw std::system_error(errno, std::system_category());
}

bool s21::socket::is_valid() const { return sd_ != INVALID_SOCKET; }

s21::socket::socket(SOCKET raw) : sd_(raw) {}
