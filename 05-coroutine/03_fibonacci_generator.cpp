#include <iostream>
#include <generator>

/*
코루틴은 C++ 20에서 도입 됐지만, 제대로 활용하려면
C++ 버전을 23 혹은 그 이상으로 맞춰야 한다. 또한 컴파일러의 종류에 따라서 C++ 23으로
맞춰져 있더라도 해당 컴파일러에서는 std::generator 같은 코루틴 객체들이 구현이 안 돼 있을 수도 있다.
테스트 결과는 다음과 같다.
Windows : MSVC(Visual Studio 2026) 사용 가능,
M1 Ultra chip Mac Studio : Apple Clang 사용 불가능,
Linux(Ubuntu) : 테스트 미완.

다음은 C++ Coroutine 이 무엇이고, 언제 사용하는지에 대한 챗 GPT의 요약이다.

>>> Coroutine은
함수의 실행 상태와 위치를 보존한 채
일시 중단(suspend)하고, 나중에 중단된 지점부터 다시 실행(resume)할 수 있게 한다.
이 특성을 이용해 비동기 I/O, generator 등의 기능을 자연스럽게 표현할 수 있다.

보통의 함수에는 자신의 실행 상태를 보존한 채 호출자에게 제어권을 넘기고,
나중에 그 지점부터 다시 실행하는 일반적인 메커니즘이 없다.
하지만, 코루틴 함수는 중간에 멈췄다가 나중에 멈췄던 부분부터 실행하는 것이 된다.

그래서 아래의 fib() 함수 안에 무한 루프가 들어가 있음에도 프로그램을 실행해보면
정상적으로 피보나치 수열 10개를 리턴하고 종료된다.


비동기 코드를 생각해보자. 예를 들면,
async_read 완료 후에 async_write 를 해야 한다고 가정하자.
아마 아래와 같이 async 어쩌고가 안쪽으로 계속 중첩되는 코드를 써야 할 것이다.

---------
async_read(socket, buffer,
    [](error_code ec, size_t n) {
        // read 완료
        async_write(socket, ...,
            [](error_code ec, size_t n) {

                // write 완료

            }); // async_write 범위 소괄호.

    }); // async_read 범위 소괄호.
---------
코루틴을 쓰면 이와 같은 비동기 코드를 serialized 하게(즉, 쉽게) 작성할 수 있다.
Task handle() {
    auto data = co_await async_read(...);
    auto result = process(data);

    co_await async_write(result);

    co_return;
}
----------

코루틴의 진짜 장점은 아래와 같다고 할 수 있다.
>>>
"비동기적으로 실행해야 하는 작업의 상태와 실행 위치를 보존하면서,
코드를 순차적인 흐름처럼 표현할 수 있게 해주는 것."
--------

단, 코루틴 기능은 직접 구현하기 보다는 최대한 std library 나 다른 라이브러리를 쓰는
것이 좋다.
코루틴을 실제로 작동하게 만들어주는 구현체를 직접 구현하는 것은
'생각보다 매우 많은 것들'을 내가 '정확하게' 정의해 줘야 하기 때문이다.

std::generator 도 std:: 를 보면 알 수 있듯이 C++ 23의 표준 라이브러리 기능 중 하나다.
coroutine을 '편리하게' 사용하려면 각자의 개발환경에 맞는 컴파일러를
C++23 버전으로 맞추는 것을 강력히 권한다.
 */

std::generator<long, long> fib() {
  long long a = 0;
  long long b = 1;

  // 무한 루프다.
  while (true) {
    auto next = a + b;

    // next를 caller에게 전달하고 coroutine을 일시정지(suspend)한다.
    // 이후 resume되면 co_yield 다음 줄부터 실행을 계속한다.
    co_yield next;
    // 다음 호출 때는 함수를 처음부터 실행하는게 아니라, co_yield 다음 줄부터 resume 된다.
    // 그 결과, 직전 함수 호출의 결과를 바탕으로 이번 호출의 피보나치수를 계산하기 위한
    // 준비를 하고(a=b; b=next; 부분),
    a = b;
    b = next;

    // 이번 호출에서는 while 루프를 다시 진입해서 co_yield에서 next를 리턴한 다음
    // 또 일시정지하는 '순환'이 완성된다.
    // 그 '순환'에는 Caller(fib()를 필요로 하는 사용자다)가 개입돼 있다.
  }//wh
}

int main() {
  int how_many_fibonacci = 10;
  int count = 0;

  // fib()를 한 번 호출하여 generator 객체를 얻는다.
  // 이후 range-for가 반복자를 이용해서 coroutine을 반복적으로 resume하면서
  // 값을 하나씩 얻는다.
  for (long value : fib()) {
    std::cout << value << " ";

    count++;
    if (count >= how_many_fibonacci) {
      break;
    }
  }

  return 0;
}