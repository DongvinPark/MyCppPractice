#include<iostream>
#include<thread>
#include<boost/asio.hpp>

using namespace std;

/*
boost.asio 의 첫 번째 튜토리얼인 Steady Timer다.
동일한 타이머 객체를 sync-blocking 으로 실행해본 후,
async-nonblocking으로도 실행해 본다.

비동기-논블록킹 모드로 실행할 경우, 타이머가 돌아가는 5초의 시간 동안에
메인 스레드가 블록킹 되지 않으므로 '딴짓'을 할 수 있다.
*/
void runSyncBlockingTimer() {
	boost::asio::io_context io;
	boost::asio::steady_timer t(io, boost::asio::chrono::seconds(5));
	cout << "sync blocking timer was initialized. you will be blocked for 5 seconds.\n";
	t.wait();
	cout << "Sync Blocking Timer ended. You are Freed!\n";
}

static void completionHandlerPrint(const boost::system::error_code&) {
	cout << "Async Nonblocking Timer ended. The boost.asio's I/O Context finished your task!\n";
}

void runAsyncNonBlockingTimer() {
	boost::asio::io_context io;

	// work guard 를 만들어줘야 io context가 '할 일' 없는 상태에서 스스로 .run()을 종료하는 현상을 막을 수 있다.
	auto workGuard = boost::asio::make_work_guard(io);

	// io_context는 별도 스레드에서 돌린다. 그래야 main 스레드가 '딴짓'을 할 수 있다.
	std::vector<std::thread> threadVec;
	// 메인 io_context 는 보통 네트워킹과 관련된 중요한 작업을 처리하는 것에 사용하므로, CPU 코어 개수만큼의 스레드를 만들어서
	// io_context.run();이 병렬 실행되게 만든다.
	int cpuCoreCnt = static_cast<int>(std::thread::hardware_concurrency());
	for(int i = 0; i < cpuCoreCnt; ++i)
	{
		threadVec.emplace_back( [&io](){io.run();} );
	}

	// async 타이머 설정. 타이머 생성에 별도의 스레드를 할당하지 않는다.
	boost::asio::steady_timer t(io, boost::asio::chrono::seconds(5));
	t.async_wait(&completionHandlerPrint);
	cout << "\nAsync Non-blocking timer was initialized. you can do other task for 5 seconds.\n";

	// 여기의 for 루프가 바로 '딴짓'을 하는 부분이다.
	// 메인 스레드가 블록 되지 않았기 때문에 이런 '딴짓'이 가능하다.
	for (int i = 0; i < 5; ++i) {
		cout << "doing count test in main thread : Non-Blocking! " << i + 1 << "\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(900));
	}

	// 프로그램 정상 종료 준비.
	for (auto& thread : threadVec)
	{
		workGuard.reset(); // 여기서 work guard 를 리셋해줘야 스레드 조인 후 프로그램이 정상 종료 된다.
		thread.join(); // 스레드 조인 시키지 않으면 프로그램이 다 실행 되고 난 후에 SIGABART 에러 뜨면서 종료된다.
	}
}

///////////////////
int main() {
	
	runSyncBlockingTimer();

	runAsyncNonBlockingTimer();

	return 0;
}
