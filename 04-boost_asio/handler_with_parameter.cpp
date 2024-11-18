#include<functional>
#include<iostream>
#include <boost/asio.hpp>

static void otherHandler(
    const boost::system::error_code& /*e*/,
    boost::asio::steady_timer* t,
    int* count
) {
    std::cout << "Do nothing. Program will be terminated.\n";
}

static void myCompletionHandler(
    const boost::system::error_code& /*e*/,
    boost::asio::steady_timer* t,
    int* count
) {
    // 여기서 if문 내의 조건을 그냥 true로 바꾸면 시간이 끝없이 0,1,2,3,4,5,6.... 이렇게 계속 출력된다.
    // 그 이유는, 동일한 핸들러를 이용해서 새로운 async_wait()를 시키는 것이 재귀적 구조를 만들기 때문이다.
    // 그래서 별도의 for, while 같은 반복문 없이도 시간이 계속 표시될 수 있었던 것이다.
    // boost.asio 에서 async_wait() 같은 함수를 호출하는 것은 io_context 내의 비동기 태스크 큐한테
    // 계속 새로운 비동기 태스크를 enqueue 하는 것과 같다.
    // 재귀 구조가 되지 않도록, 위에서 정의한 'otherHandler()'함수를 
    // t->async_wait(std::bind(myCompletionHandler, boost::asio::placeholders::error, t, count));
    // 부분에서 myCompletionHandler 대신 집어 넣으면, 비동기 태스크는 더 이상 진행되지 않고 프로그램이 종료된다.
    if (*count < 5) {
        std::cout << *count << "\n";
        ++(*count);
        t->expires_at(t->expiry() + boost::asio::chrono::seconds(1));

        // 이 코드는 1.86 버전에서는 작동하지만, 1.74 버전에서는 작동하지 않는다.
        //t->async_wait(std::bind(myCompletionHandler, boost::asio::placeholders::error, t, count));

        // 이 코드는 윈도우, 리눅스, 맥, WSL 환경에서 모두 작동한다. 람다를 사용해서 재정의 했다.
        t->async_wait([t, count](const boost::system::error_code& error) {
            myCompletionHandler(error, t, count);
        });

    }
}

/*
0부터 4를 1초 간격으로 출력하는 타이머 예제다. 기억해야 하는 점은 다음의 3 가지다.

1. completion handler에게 나의 커스텀 인자에 해당하는 count를 넘겨준다는 것

2. completion handler 내에서 자기 자신을 async 호출함으로써 일종의 재귀 루프를 만들 수 있다는 것

3. completion handler 의 시그니처를 맞춰주기 위한 트릭으로써 std::bind와 boost:asio::placeholder::error를 쓴다는 것
*/
int main() {
    boost::asio::io_context io;
    int count = 0;
    boost::asio::steady_timer t(io, boost::asio::chrono::seconds(1));

    // async_wait()함수를 호출할 때, std::bind()를 사용하는 이유는,
    // boost.asio에서 completion handler 함수 또는 함수 객체의 시그니처가
    // void(const boost::system::error_code&) 이와 같기를 기대하기 때문이다.
    // 따라서 이러한 '기대'를 충족시켜주기 위해서 error_code를 포함한 다른 파라미터들을 std::bind로 묶어주는 것.
    // 단, 이렇게 std::bind를 이용해서 여러 파라미터들을 하나의 파라미터인 척 할 수 있게끔 묶어 줄 때는
    // boost::asio::placeholder::error 이 부분을 반드시 포함해야 한다.
    // 그래야, 현재 함수 호출 내부에서 받은 파라미터들을 사용한 후, 
    // error_code를 포함한 적절한 시그니처로 다음 async_wait()를 호출할 수 있다.

    // 참고로, 이 코드는 boost library 1.86 버전에서는 작동하지만, WSL with 1.74 버전에서는 작동하지 않는다.
    //t.async_wait(std::bind(myCompletionHandler, boost::asio::placeholders::error, &t, &count));

    // 이 코드는 윈도우, 리눅스, 맥, WSL 환경에서 모두 작동한다.
    // 위의 std::bind 코드의 경우, boosd.asio의 버전이 달라지거나 컴파일러 구현체가 달라질 경우
    // 예상치 못한 에러를 뿜어낼 수 있다.
    // 그러나, lambda를 사용할 경우 캡쳐 리스트를 사용해서 람다 내부에서 사용해야 하는 참조들을 전달함과 동시에,
    // (const boost::system::error_code& error)라는 boost.asio 용 completion handler 함수 시그니처를 정확하게
    // 맞춰줄 수 있고, lambda는 C++11 부터 지원하기 때문에 backward compatibility가 뛰어나다.
    // 따라서, 웬만하면 lambda를 이용해서 boost.asio 의 completion handler를 구현하도록 하자.
    t.async_wait([&t, &count](const boost::system::error_code& error) {
        myCompletionHandler(error, &t, &count);
    });


    io.run();

    return 0;
}
