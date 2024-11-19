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
    
    void startTimer(std::vector<int>& vec){
        // 이 코드는 물론 잘 작동한다. 그러나, boost.asio 1_74 버전과 같은 이전 버전에서는 구현의 차이에 의해서
        // 컴파일이 안 될 가능성이 크다.
        //timer_.async_wait(std::bind(&printer::print, this));
        
        // 따라서, 아래의 lambda를 이용해서 호출하자.
        timer_.async_wait([&vec ,this](const boost::system::error_code&){print(vec);});
    }
    
    void print(std::vector<int>& vec){
        if(count_<5){
            std::cout << count_ << "\n";
            vec.push_back(count_*count_);
            ++count_;
            
            timer_.expires_at(timer_.expiry() + boost::asio::chrono::seconds(1));
            
            // lambda로 대체하자.
            //timer_.async_wait(std::bind(&printer::print, this));
            timer_.async_wait([&vec, this](const boost::system::error_code&){print(vec);});
        }
    }
    
private:
    boost::asio::steady_timer timer_;
    int count_;
};//end of printer

int main(){
    boost::asio::io_context io;
    
    std::vector<int> squares;
    printer p(io);
    p.startTimer(squares);
    
    io.run();
    
    std::cout << "timer expired." << "\n" << "elems in vector is : ";
    for (int v : squares) {
        std::cout << v << ", ";
    }
    std::cout << "\n";
    return 0;
}

