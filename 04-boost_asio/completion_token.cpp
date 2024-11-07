#include <boost/asio.hpp>
#include <chrono>
#include <iostream>

using boost::system::error_code;
using boost::asio::steady_timer;
namespace asio = boost::asio;

/*
completion token 의 역할을 이해하기 위한 목적으로 Chat GPT에게 작성을 요청한 예제이다.
메인 함수에 진입한 직후로부터 약 1초의 시간이 지나면 "Timer completed successfuly."
또는 "Timer error: ..."라는 메시지가 나온 후 종료 된다.

이 코드에서 주목할 점은, 실제 돌아가는 스레드는 메인 스레드 1개 뿐인데,
메인 스레드는 async_wait_some() 함수한테 일을 맡겨놓고는 자기는 blocking 되지 않고
자기가 해야 하는 다른 할 일을 하다가 async_wait_some() 함수가 할 일을 끝내면 그 결과를
받아서 일을 처리한다는 점이다. 즉, 멀티 스레드의 도움 없이 async 동작을 하고 있는 것이고,
이것이 boost.asio가 지향하는 '비동기'이다.

boost.asio에서 completion token의 역할을 정확히 알고 싶다면 아래의 문서를 참고한다.
이 문서는 boost.asio 를 구현한 개발자가 직접 작성한 문서다.
https://www.boost.org/doc/libs/1_86_0/doc/html/boost_asio/overview/model/completion_tokens.html

boost.asio 의 overview를 보고 싶다면 아래의 문서를 참고한다.
https://www.boost.org/doc/libs/1_86_0/doc/html/boost_asio.html
*/

template <typename CompletionToken>
auto async_wait_some(
    steady_timer& timer,
    std::chrono::milliseconds duration,
    CompletionToken&& token)
{
    // Set the timer to expire after the specified duration.
    timer.expires_after(duration);
    // Initiate the async_wait operation on the timer.
    return timer.async_wait(std::forward<CompletionToken>(token));
}

int main()
{
    std::cout << "entered main!\n";

    asio::io_context io;
    steady_timer timer(io);

    async_wait_some(
        timer,
        std::chrono::milliseconds(1'000),
        
        // 아래의 람다식이 바로 async_wait_some 함수의 세 번째 인자이자 completion token이다.
        [](const error_code& ec) {
            if (!ec) {
                // 이게 java의 System.currentTimeMillis()의 C++ 버전이다.
                long milliseconds_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch()
                ).count();
                std::cout << "\nTimer completed successfully. epoch millisec : " << milliseconds_since_epoch << "\n";
            }
            else {
                std::cout << "Timer error: " << ec.message() << "\n";
            }
        }
    );

    for (int i = 0; i < 10; i++) {
        std::cout << "do other work in main! : ";
        long milliseconds_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
        std::cout << "Current time in milliseconds since epoch: " << milliseconds_since_epoch << " ms\n";
        // 이게 java의 Thread.sleep();과 비슷한 기능이다.
        std::this_thread::sleep_for(std::chrono::milliseconds(90));
    }

    io.run();
    return 0;
}

