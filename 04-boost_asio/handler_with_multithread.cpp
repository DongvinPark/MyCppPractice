#include <functional>
#include <iostream>
#include <thread>
#include <boost/asio.hpp>

class printer {
public:
	printer(boost::asio::io_context& io) : 
		strand_(boost::asio::make_strand(io)),
		timer1_(io, boost::asio::chrono::seconds(1)),
		timer2_(io, boost::asio::chrono::seconds(1)),
		count_(0){}

	~printer() {
		std::cout << "Final count is : " << count_ << "\n";
	}

	void startTimers(std::vector<int>& vec, const int limit) {
		timer1_.async_wait(boost::asio::bind_executor(
			strand_, [this, &vec, limit](const boost::system::error_code&) {print1(vec, limit); })
		);
		timer1_.async_wait(boost::asio::bind_executor(
			strand_, [this, &vec, limit](const boost::system::error_code&) {print2(vec, limit); })
		);
	}

	void print1(std::vector<int>& vec, const int limit) {
		if (count_ < limit) {
			std::cout << "Timer 1: " << count_ << ", cur thread id : " << std::this_thread::get_id() << "\n";
			vec.push_back(count_ * count_);
			++count_;
			timer1_.expires_at(timer1_.expiry() + boost::asio::chrono::seconds(1));
			timer1_.async_wait(
				boost::asio::bind_executor(
					strand_, [this, &vec, limit](const boost::system::error_code&) {print1(vec, limit); }
				)
			);
		}
	}

	void print2(std::vector<int>& vec, int limit) {
		if (count_ < limit) {
			std::cout << "Timer 2: " << count_ << ", cur thread id : " << std::this_thread::get_id() << "\n";
			vec.push_back(count_*count_);
			++count_;
			timer2_.expires_at(timer2_.expiry() + boost::asio::chrono::seconds(1));
			timer2_.async_wait(
				boost::asio::bind_executor(
					strand_, [this, &vec, limit](const boost::system::error_code&) {print2(vec, limit); }
				)
			);
		}
	}

private:
	boost::asio::strand<boost::asio::io_context::executor_type> strand_;
	boost::asio::steady_timer timer1_;
	boost::asio::steady_timer timer2_;
	int count_;
};

/*
하나의 단일 io_context를 두 개의 스레드에서 동시에 사용하여
각각의 스레드에서 1초에 1번씩 2 개의 타이머에서 각각 async_wait()를 호출한다.
이러한 동시적 호출로 인한 race condition과 관련 리소스( std::vector<int>, count_ )의
쓰기 누락 문제를 strand 객체로 해결하는 예제다.

간략하게 설명하면, boost::asio::io_context io;는 일종의 태스크 큐 역할을 하고,
이 '큐'에 쌓인 태스크들을 serialize 해주는 것이 boost::asio::strand 다.

따라서, boost::asio::io_context io 객체를 여러 개의 스레드들이 마구 호출해도
항상 정확한 결과를 보여준다.

native Window에서 실행해본 결과는 아래와 같다. 자세히 보면,
2 개의 스레드에서 각각 두 개의 timer_.async_wait()를 호출하여 io_context에 태스크가 쌓이지만,
그것들이 순서대로 정확하게 처리되고 있음을 알 수 있다.
단, 실행 플랫폼(WSL, native Ubuntu Linux, MacOS, native Window)에 따라서
스레드 아이디 번호 값과 나타나는 순서가 뒤섞일 수 있다.

Timer 2: 0, cur thread id : 17400
Timer 1: 1, cur thread id : 33648
Timer 2: 2, cur thread id : 33648
Timer 1: 3, cur thread id : 17400
Timer 2: 4, cur thread id : 17400
Timer 1: 5, cur thread id : 33648
Timer 2: 6, cur thread id : 33648
Timer 1: 7, cur thread id : 17400
Timer 2: 8, cur thread id : 17400
Timer 1: 9, cur thread id : 33648
Two threads joined main thread.

Elemes of vectors are,
0, 1, 4, 9, 16, 25, 36, 49, 64, 81,
Final count is : 10

*/

int main() {
	boost::asio::io_context io;
	
	std::vector<int> squareVec;
	int limit = 10;

	printer p(io);
	p.startTimers(squareVec, limit);
	
	std::thread t1([&] {io.run(); });
	std::thread t2([&] {io.run(); });

	t1.join();
	t2.join();

	std::cout << "Two threads joined main thread.\n\n";
	std::cout << "Elemes of vectors are, \n";
	for (int v : squareVec) {
		std::cout << v << ", ";
	}
	std::cout << "\n";

	return 0;
}
