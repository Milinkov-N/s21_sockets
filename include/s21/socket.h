#pragma once

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#include <cstdint>
#include <string_view>
#include <vector>

#define STDCXX_17_OR_HIGHER (__cplusplus >= 201703L || _MSVC_LANG >= 201703L)

#ifndef _WIN32
typedef int SOCKET;

#define SD_BOTH SHUT_RDWR
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET (SOCKET)(~0)
#endif

#ifndef SOCKET_ERROR
#define SOCKET_ERROR (SOCKET)(-1)
#endif

#if _WIN32
#define errno WSAGetLastError()
#endif

namespace s21 {
/* The address family. Possible values for the address family are defined in
 * the Winsock2.h header file.
 */
enum family : uint16_t {
  /* The address family is unspecified. */
  unset = AF_UNSPEC,

  /* The Internet Protocol version 4 (IPv4) address family. */
  ipv4 = AF_INET,

/* The NetBIOS address family. This address family is only supported if a
 * Windows Sockets provider for NetBIOS is installed.
 */
#ifdef _WIN32
  netbios = AF_NETBIOS,
#endif

  /* The Internet Protocol version 6 (IPv6) address family. */
  ipv6 = AF_INET6,

  /* The Infrared Data Association (IrDA) address family. This address family
   * is only supported if the computer has an infrared port and driver
   * installed.
   */
  irda = AF_IRDA,

/* The Bluetooth address family. This address family is only supported if a
 * Bluetooth adapter is installed on Windows Server 2003 or later.
 */
#ifdef _WIN32
  bluetooth = AF_BTH,
#endif
};

enum type {
  /* Provides sequenced, reliable, two-way, connection-based byte streams with
   * an OOB data transmission mechanism. Uses the Transmission Control
   * Protocol (TCP) for the Internet address family (`AF_INET` or `AF_INET6`).
   * If the `ai_family` member is `AF_IRDA`, then `SOCK_STREAM` is the only
   * supported socket type.
   */
  stream = SOCK_STREAM,

  /* Supports datagrams, which are connectionless, unreliable buffers of a
   * fixed (typically small) maximum length. Uses the User Datagram Protocol
   * (UDP) for the Internet address family (`AF_INET` or `AF_INET6`).
   */
  datagram = SOCK_DGRAM,

  /* Provides a raw socket that allows an application to manipulate the next
   * upper-layer protocol header.
   */
  raw = SOCK_RAW,

  /* Provides a reliable message datagram. An example of this type is the
   * Pragmatic General Multicast (PGM) multicast protocol implementation in
   * Windows, often referred to as reliable multicast programming.
   */
  rdm = SOCK_RDM,

  /* Provides a pseudo-stream packet based on datagrams. */
  seqpacket = SOCK_SEQPACKET,
};

enum protocol {
  /* The Transmission Control Protocol (TCP). This is a possible value when
   * the `ai_family` member is `AF_INET` or `AF_INET6` and the ai_socktype
   * member is `SOCK_STREAM`.
   */
  tcp = IPPROTO_TCP,

  /* The User Datagram Protocol (UDP). This is a possible value when the
   * ai_family member is `AF_INET` or `AF_INET6` and the type parameter is
   * `SOCK_DGRAM`.
   */
  udp = IPPROTO_UDP,
};

class socket {
 public:
  using Family = family;
  using Type = type;
  using Protocol = protocol;

 public:
  socket();
  socket(family, type, protocol = tcp);
  socket(const socket&) = delete;
  socket(socket&&) noexcept;
  ~socket();

 public:
  socket& operator=(const socket&) = delete;
  socket& operator=(socket&&) noexcept;

 public:
#if STDCXX_17_OR_HIGHER
  void bind(std::string_view, uint16_t, family = ipv4);
#else
  void bind(const char*, uint16_t, family = ipv4);
#endif

#if STDCXX_17_OR_HIGHER
  void connect(std::string_view, uint16_t, family = ipv4);
#else
  void connect(const char*, uint16_t, family = ipv4);
#endif

  void listen(int = SOMAXCONN);
  [[nodiscard]] socket accept();
  std::vector<uint8_t> recieve(size_t);

#if STDCXX_17_OR_HIGHER
  int send(std::string_view);
#else
  int send(const std::vector<uint8_t>&);
#endif

 public:
  bool is_valid() const;

 private:
  socket(SOCKET);

  void shutdown();

 private:
  SOCKET sd_{INVALID_SOCKET};
  bool connected_{false};
};
}  // namespace s21
