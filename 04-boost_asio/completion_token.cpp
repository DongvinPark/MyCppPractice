#include <boost/asio.hpp>
#include <chrono>
#include <iostream>

using boost::system::error_code;
using boost::asio::steady_timer;
namespace asio = boost::asio;

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

/*
completion token 의 역할을 이해하기 위한 목적으로 Chat GPT에게 작성을 요청한 예제이다.
메인 함수에 진입한 직후로부터 약 1초의 시간이 지나면 "Timer completed successfuly."
또는 "Timer error: ..."라는 메시지가 나온 후 종료 된다.

boost.asio에서 completion token의 역할을 정확히 알고 싶다면 아래의 문서를 참고한다.
이 문서는 boost.asio 를 구현한 개발자가 직접 작성한 문서다.
https://www.boost.org/doc/libs/1_86_0/doc/html/boost_asio/overview/model/completion_tokens.html

boost.asio 의 overview를 보고 싶다면 아래의 문서를 참고한다.
https://www.boost.org/doc/libs/1_86_0/doc/html/boost_asio.html
*/
int main()
{
    std::cout << "entered main!\n";

    asio::io_context io;
    steady_timer timer(io);

    async_wait_some(timer, std::chrono::milliseconds(1'000),
        [](const error_code& ec) {
            if (!ec) {
                std::cout << "Timer completed successfully.\n";
            }
            else {
                std::cout << "Timer error: " << ec.message() << "\n";
            }
        }
    );

    io.run();
    return 0;
}

