#include<iostream>
#include<boost/asio.hpp>

using namespace std;

void runSyncBlockingTimer() {
	boost::asio::io_context io;
	boost::asio::steady_timer t(io, boost::asio::chrono::seconds(5));
	cout << "timer init completed. you will be blocked for 5 seconds.\n";
	t.wait();
	cout << "Sync Blocking Timer ended. You are Freed!\n";
}

static void completionHandlerPrint(const boost::system::error_code&) {
	cout << "Async Nonblocking Timer ended. The boost.asio's I/O Context finished your task!\n";
}

void runAsyncNonBlockingTimer() {
	boost::asio::io_context io;
	boost::asio::steady_timer t(io, boost::asio::chrono::seconds(5));
	t.async_wait(&completionHandlerPrint);
	cout << "\ntimer init completed. you can do what you want for 5 seconds.\n";
	
	// 여기의 for 루프가 바로 '딴짓'을 하는 부분이다.
	// 메인 스레드가 블록 되지 않았기 때문에 이런 '딴짓'이 가능하다.
	for (int i = 0; i < 5; ++i) {
		cout << "doing count test in Non-Blocking : " << i + 1 << "\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(900));
	}

	// 한 가지 주의할 점은, io를 run() 시키는 부분이 '딴짓'하는 부분보다 나중에 나와야 한다는 점이다.
	// io.run()이 for 문 보다 앞에 위치하게 되면 5초가 다 지나고 나서야 '딴짓' 이 시작되며,
	// io.run()이 아예 호출되지 않을 경우, competion handler의 역할을 하는
	// static void completionHandlerPrint(const boost::system::error_code&) {...} 함수가
	// 5초의 시간이 지나도 아예 호출되지 않는다.
	io.run();
}

/*
boost.asio 의 첫 번째 튜토리얼인 Steady Timer다.
동일한 타이머 객체를 sync-blocking 으로 실행해본 후,
async-nonblocking으로도 실행해 본다.
비동기-논블록킹 모드로 실행할 경우, 타이머가 돌아가는 5초의 시간 동안에
메인 스레드가 블록킹 되지 않으므로 '딴짓'을 할 수 있다.
*/
int main() {
	
	runSyncBlockingTimer();

	runAsyncNonBlockingTimer();

	return 0;
}
