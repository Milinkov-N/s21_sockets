#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>

#include <cstdint>
#include <string_view>
#include <vector>

namespace s21 {

/* Flags that indicate options used in the `getaddrinfo()` function. */
enum flags {
  /* The socket address will be used in a call to the `bind()` function. */
  passive = AI_PASSIVE,

  /* The canonical name is returned in the first `ai_canonname` member. */
  canonname = AI_CANONNAME,

  /* The nodename parameter passed to the `getaddrinfo()` function must be a
   * numeric string.
   */
  numerichost = AI_NUMERICHOST,

  /* If this bit is set, a request is made for IPv6 addresses and IPv4 addresses
   * with `AI_V4MAPPED`.
   */
  all = AI_ALL,

  /* The `getaddrinfo()` will resolve only if a global address is configured.
   * The IPv6 and IPv4 loopback address is not considered a valid global
   * address.
   */
  addrconfig = AI_ADDRCONFIG,

  /* If the `getaddrinfo()` request for IPv6 addresses fails, a name service
   * request is made for IPv4 addresses and these addresses are converted to
   * IPv4-mapped IPv6 address format.
   */
  v4mapped = AI_V4MAPPED,

  /* The address information can be from a non-authoritative namespace provider.
   * This option is only supported on Windows Vista and later for the `NS_EMAIL`
   * namespace.
   */
  nonauth = AI_NON_AUTHORITATIVE,

  /* The address information is from a secure channel.
   * This option is only supported on Windows Vista and later for the `NS_EMAIL`
   * namespace.
   */
  secure = AI_SECURE,

  /* The address information is for a preferred name for a user.
   * This option is only supported on Windows Vista and later for the NS_EMAIL
   * namespace.
   */
  return_preferred_names = AI_RETURN_PREFERRED_NAMES,

  /* If a flat name (single label) is specified, `getaddrinfo()` will return the
   * fully qualified domain name that the name eventually resolved to. The fully
   * qualified domain name is returned in the `ai_canonname member`. This is
   * different than `AI_CANONNAME` bit flag that returns the canonical name
   * registered in DNS which may be different than the fully qualified domain
   * name that the flat name resolved to.
   *
   * Only one of the `AI_FQDN` and `AI_CANONNAME` bits can be set. The
   * `getaddrinfo()` function will fail if both flags are present with
   * `EAI_BADFLAGS`.
   *
   * This option is supported on Windows 7, Windows Server 2008 R2, and later.
   */
  fqdn = AI_FQDN,

  /* A hint to the namespace provider that the hostname being queried is being
   * used in a file share scenario. The namespace provider may ignore this hint.
   * This option is supported on Windows 7, Windows Server 2008 R2, and later.
   */
  fileserver = AI_FILESERVER,
};

/* The address family. Possible values for the address family are defined in the
 * Winsock2.h header file.
 */
enum family {
  /* The address family is unspecified. */
  unset = AF_UNSPEC,

  /* The Internet Protocol version 4 (IPv4) address family. */
  ipv4 = AF_INET,

  /* The NetBIOS address family. This address family is only supported if a
   * Windows Sockets provider for NetBIOS is installed.
   */
  netbios = AF_NETBIOS,

  /* The Internet Protocol version 6 (IPv6) address family. */
  ipv6 = AF_INET6,

  /* The Infrared Data Association (IrDA) address family. This address family is
   * only supported if the computer has an infrared port and driver installed.
   */
  irda = AF_IRDA,

  /* The Bluetooth address family. This address family is only supported if a
   * Bluetooth adapter is installed on Windows Server 2003 or later.
   */
  bluetooth = AF_BTH,
};

enum type {
  /* Provides sequenced, reliable, two-way, connection-based byte streams with
   * an OOB data transmission mechanism. Uses the Transmission Control Protocol
   * (TCP) for the Internet address family (`AF_INET` or `AF_INET6`). If the
   * `ai_family` member is `AF_IRDA`, then `SOCK_STREAM` is the only supported
   * socket type.
   */
  stream = SOCK_STREAM,

  /* Supports datagrams, which are connectionless, unreliable buffers of a fixed
   * (typically small) maximum length. Uses the User Datagram Protocol (UDP) for
   * the Internet address family (`AF_INET` or `AF_INET6`).
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
  /* The Transmission Control Protocol (TCP). This is a possible value when the
   * `ai_family` member is `AF_INET` or `AF_INET6` and the ai_socktype member is
   * `SOCK_STREAM`.
   */
  tcp = IPPROTO_TCP,

  /* The User Datagram Protocol (UDP). This is a possible value when the
   * ai_family member is `AF_INET` or `AF_INET6` and the type parameter is
   * `SOCK_DGRAM`.
   */
  udp = IPPROTO_UDP,
};

class SockAddr {
 public:
  SockAddr();
  SockAddr(family, std::string_view, uint16_t);
  SockAddr(const SockAddr&) = default;
  SockAddr(SockAddr&&) = default;
  ~SockAddr();

 public:
  SockAddr& operator=(const SockAddr&) = default;
  SockAddr& operator=(SockAddr&&) = default;

 public:
  inline family family() const { return family_; }
  inline std::string_view addr() const { return addr_; }

  /* @returns `unsigned short` of port value parsed with `htons()` function  */
  inline uint16_t port() const { return htons(port_); }

  struct sockaddr_in sockaddr_in() const;

 private:
  s21::family family_;
  std::string_view addr_;
  uint16_t port_;
};

class socket {
 public:
  using Family = s21::family;
  using Type = s21::type;
  using Protocol = s21::protocol;

 public:
  socket();
  socket(s21::family, type, protocol = tcp);
  socket(const socket&) = delete;
  socket(socket&&) noexcept;
  ~socket();

 public:
  socket& operator=(const socket&) = delete;
  socket& operator=(socket&&) noexcept;

 public:
  void bind(SockAddr);
  void connect(SockAddr);
  void listen(int = SOMAXCONN);
  [[nodiscard]] socket accept();
  std::vector<uint8_t> recieve(size_t);
  int send(std::string_view);

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
