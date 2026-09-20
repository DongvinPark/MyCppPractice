#include <iostream>
#include <generator>

std::generator<long, long> fib() {
  long long a = 0;
  long long b = 1;

  while (true) {
    auto next = a + b;
    co_yield next;
    a = b;
    b = next;
  }//wh

  //co_return 0;
}

void use_coroutine(int max) {
  int count = 0;

  for (long value : fib()) {
    std::cout << value << " ";

    count++;
    if (count >= max) {
      break;
    }
  }
}

int main() {
  use_coroutine(10);
  return 0;
}