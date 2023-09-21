# SockLib

Small and very simple abstraction of Winsock library. Made mostly for personal use.

## Example

```cpp
#include <iostream>
#include <string>
#include <socklib/socklib.h>

int main(void) {
    sock::WS2Lib ws2; // initializing the winsock2 library
    sock::Socket listener(sock::Family::Ipv4, sock::Type::Stream);

    if (listener.is_valid()) {
        listener.bind(sock::SockAddr(sock::Family::Ipv4, "127.0.0.1", 3000));
        listener.listen();

        auto conn = listener.accept();

        auto bytes = conn.recieve(128);
        std::string str(bytes.begin(), bytes.end());
        std::cout << str << std::endl;
    }

    return 0;
}
```
