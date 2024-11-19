#include <functional>
#include <iostream>
#include <boost/asio.hpp>

class printer {
public:
    printer(boost::asio::io_context& io)
    : timer_(io, boost::asio::chrono::seconds(1)), count_(0) {}
    
    ~printer(){
        std::cout << "\nDestructor called. Final count is : " << count_ << "\n";
    }
    
    void startTimer(std::vector<int>& vec, int repeatCnt){
        // 이 코드는 물론 잘 작동한다. 그러나, boost.asio 1_74 버전과 같은 이전 버전에서는 구현의 차이에 의해서
        // 컴파일이 안 될 가능성이 크다.
        //timer_.async_wait(std::bind(&printer::print, this));
        
        // 따라서, 아래의 lambda를 이용해서 호출하자.
        timer_.async_wait([&vec ,this, repeatCnt](const boost::system::error_code&){print(vec, repeatCnt);});
    }
    
    void print(std::vector<int>& vec, int repeatCnt){
        if(count_<repeatCnt){
            std::cout << count_ << "\n";
            vec.push_back(count_*count_);
            ++count_;
            
            timer_.expires_at(timer_.expiry() + boost::asio::chrono::seconds(1));
            
            // lambda로 대체하자.
            //timer_.async_wait(std::bind(&printer::print, this));
            
            // 클래스 멤버 함수와 lambda를 이용해서 async_wait() 같은 boost.asio 라이브러리 함수들을 호출하기 위해서는
            // lambda의 캡쳐 리스트에 현재 클래스의 포인터인 'this'를 꼭 써줘야 한다.
            timer_.async_wait([&vec, this, repeatCnt](const boost::system::error_code&){print(vec, repeatCnt);});
        }
    }
    
private:
    boost::asio::steady_timer timer_;
    int count_;
};//end of printer

/*
 boost.asio 의 async_wait()를 호출할 때 사용할 completion handler로
 클래스의 멤버 함수를 사용함과 동시에, 해당 멤버 함수에게 클래스 바깥의 인자를 두 종류(참조 타입, 값 타입)
 전달하여 사용하는 예제이다.
 
 lambda의 캡쳐 리스트를 사용하여 completion handler에게 파라미터들을 전달할 때,
 참조 타입(std::vector<int> 등)인 경우엔 함수 시그니처와 람다 캡쳐 리스트에 전부 '&'기호를 사용하고 있고,
 값 타입(int 등)인 경우엔 함수 시그니처와 람다 캡쳐 리스트에 전부 '&' 기호가 사용되지 않았음을 알 수 있다.
*/
int main(){
    boost::asio::io_context io;
    
    std::vector<int> squareVector;
    int repeatCnt = 5;
    printer p(io);
    p.startTimer(squareVector, repeatCnt);
    
    io.run();
    
    std::cout << "timer expired." << "\n" << "elems in vector is : ";
    for (int v : squareVector) {
        std::cout << v << ", ";
    }
    std::cout << "\n";
    return 0;
}

