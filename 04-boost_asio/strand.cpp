#include<boost/asio.hpp>
#include<iostream>
#include<thread>
#include<mutex>

/*
boost.asio의 Strand 객체의 사용예시다.
두 개의 스레드가 1 개의 Counter 객체 내의 동일한 counter_ 필드를
동시에 각각 5번씩 += 1 하는데, 
그 결과가 아래와 같이 '동기화'되어 나타난다.

Counter 객채의 increment() 멤버 함수가 어느 스레드에서 호출되는지와 상관 없이
순서대로, 중복 없이, 빠짐 없이 실행시키는 것이다.
정확히는 Counter객체 내의 Strand 객체에 [this]... 람다 객체를
post하여 Strand 객체를 이용한 동기화를 달성하는 것이다.

Counter ++ result : 1 on thread: 10248
Counter ++ result : 2 on thread: 42160
Counter ++ result : 3 on thread: 42160
Counter ++ result : 4 on thread: 42160
Counter ++ result : 5 on thread: 42160
Counter ++ result : 6 on thread: 42160
Counter ++ result : 7 on thread: 42160
Counter ++ result : 8 on thread: 42160
Counter ++ result : 9 on thread: 42160
Counter ++ result : 10 on thread: 42160

Final counter value : 10

여기서 만약, 각 스레드를 초기화 하는 람다 내부의 
std::cout ... 출력문의 주석을 해제한다면, '동시 접근'에 의해
입력 연산이 정확한 순서대로 진행되지 않으면서 아래와 같이 출력 결과가 엉망이 된다.

30476
4109230476

4109230476

3047641092

4109230476

41092Counter ++ result :
1 on thread: 30476
Counter ++ result : 2 on thread: 41092
Counter ++ result : 3 on thread: 41092
Counter ++ result : 4 on thread: 41092
Counter ++ result : 5 on thread: 41092
Counter ++ result : 6 on thread: 41092
Counter ++ result : 7 on thread: 41092
Counter ++ result : 8 on thread: 41092
Counter ++ result : 9 on thread: 41092
Counter ++ result : 10 on thread: 41092

Final counter value : 10

*/

class Counter {
public:
	explicit Counter(boost::asio::io_context& io_context)
		: strand_(boost::asio::make_strand(io_context)), counter_(0) {}

	void increment() {
		boost::asio::post(
			strand_,
			// [capture list] (parameters) -> return_type { body }
			// 이게 C++ 람다의 기본 문법이다.
			// 캡쳐 리스트를 람다한테 줘야 람다 내부에서 캡쳐 리스트 참조를 이용해서
			// 람다 외부 객체에 접근할 수 있다. 아래 예시에서는 this 키워드를 썼는데,
			// 이로써 람다 내부에서 Counter 객체 내의 필드에 접근할 수 있게 된다.
			// 아래의 람다는 파라미터와 리턴 타입은 없고, 바디만 정의돼 있다.
			[this]() {
				++counter_;
				std::cout << "Counter ++ result : " << counter_
					<< " on thread: " << std::this_thread::get_id() << "\n";
			}
		);
	}

	int get_counter() const { return counter_; }

private:
	boost::asio::strand<boost::asio::io_context::executor_type> strand_;
	int counter_;
};

int main() {

	boost::asio::io_context io_context;
	Counter counter(io_context);

	std::thread t1(
		// 이것도 람다의 사용예시다. 파라미터와 리턴 타입이 생략돼 있다.
		[&io_context, &counter] {
			for (int i = 0; i < 5; ++i) {
				counter.increment();
				//std::cout << std::this_thread::get_id() << "\n";
			}
		}
	);

	std::thread t2(
		[&io_context, &counter] {
			for (int i = 0; i < 5; ++i) {
				counter.increment();
				//std::cout << std::this_thread::get_id() << "\n";
			}
		}
	);

	t1.join();
	t2.join();

    // boost.asio의 io_context를 단 한 번만 run 함으로써, 두 스레드의 작업을 모두
    // 수행하게 만든다.
    // 만약 각각의 스레드 내부에서 io_context.run()을 호출할 경우,
    // 한 스레드의 작업만 마치고 프로그램이 바로 종료돼 버리는 현상이 가끔 발생한다.
    io_context.run();

	std::cout << "\nFinal counter value : " << counter.get_counter() << "\n";

	return 0;
}
