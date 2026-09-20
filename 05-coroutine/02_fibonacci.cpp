#include <iostream>
#include <generator>

std::generator<long, long> fib() {
  long long a = 0;
  long long b = 1;
  while (a<b) {
    auto next = a + b;
    co_yield next;
    a = b;
    b = next;
  }
  co_return 0;
}

void user(int max) {
  for (int i = 0; i < max; i++) {
    std::cout << fib() << " ";
  }
}

int main() {
  user(10);
  return 0;
}