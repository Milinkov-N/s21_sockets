# S21 Sockets

Small and very simple abstraction of Winsock library. Made mostly for personal use.

## Example

```cpp
#include <iostream>
#include <string>
#include <s21/socket.h>

int main(void) {
    s21::socket listener(s21::ipv4, s21::stream);

    if (listener.is_valid()) {
        listener.bind(s21::SockAddr(s21::ipv4, "127.0.0.1", 3000));
        listener.listen();

        auto conn = listener.accept();

        auto bytes = conn.recieve(128);
        std::string str(bytes.begin(), bytes.end());
        std::cout << str << std::endl;
    }

    return 0;
}
```
