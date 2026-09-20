#include <generator>
#include <iostream>

// 1부터 n까지 숫자를 생성하는 제너레이터 함수
std::generator<int> count_to(int n) {
  for (int i = 1; i <= n; ++i) {
    co_yield i; // 값을 호출자에게 전달하고 일시 중단
  }
}

int main() {
  // range-based for 문으로 지연 생성된 값들을 순회
  for (int v : count_to(5)) {
    std::cout << v << " ";
  }
  return 0;
}