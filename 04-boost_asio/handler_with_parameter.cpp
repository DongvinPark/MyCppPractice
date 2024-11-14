#include<functional>
#include<iostream>
#include <boost/asio.hpp>

static void myCompletionHandler(
    const boost::system::error_code& /*e*/,
    boost::asio::steady_timer* t,
    int* count
){
    if(*count < 5){
        std::cout << *count << "\n";
        ++(*count);
        t->expires_at(t->expiry() + boost::asio::chrono::seconds(1));
        t->async_wait( std::bind( myCompletionHandler, boost::asio::placeholders::error, t, count) );
    }
}

/*
0부터 4를 1초 간격으로 출력하는 타이머 예제다. 달라진 점은, completion handler에게
나의 커스텀 인자에 해당하는 count를 넘겨준다는 것이다.
*/
int main(){
    boost::asio::io_context io;
    int count = 0;
    boost::asio::steady_timer t(io, boost::asio::chrono::seconds(1));
    
    t.async_wait(std::bind(myCompletionHandler, boost::asio::placeholders::error, &t, &count) );
    
    io.run();
    
    return 0;
}

