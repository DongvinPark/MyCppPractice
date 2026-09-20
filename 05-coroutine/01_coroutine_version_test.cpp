#include <iostream>

struct Test {
};

int main() {
  std::cout << "__cplusplus = " << __cplusplus << '\n';

  // 이건 MSVC 용.
  std::cout << "_MSVC_LANG = " << _MSVC_LANG << '\n';
}