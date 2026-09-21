#include <iostream>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

/*
작업을 진행 중인 std::jthread 에게 stop_token을 이용해서 중간에 작업을 멈추게 만드는 예제다.
C++ 20 버전 부터 실행 가능하다.
std::jthread 는 별도의 join을 해주지 않아도 범위를 벗어나면 자동으로 조인이 되기 때문에 안전하다.

만약 jthread 를 이용할 수 없는 C++ 버전(20 미만)이라면, thread 용 태스크 안에
std::atomic<bool> cancelled;
같은 플래그를 만들어서 이걸로 스레드를 컨트롤 해야 한다.
 */

class BigIntegerProducer
{
private:
 std::queue<long long> queue;
 std::mutex mutex;
 std::condition_variable_any cv;

 static constexpr size_t MAX_SIZE = 10;

public:
 void run(std::stop_token stopToken)
 {
  long long p = 0;

  while (!stopToken.stop_requested())
  {
   std::unique_lock<std::mutex> lock(mutex);

   // wait until: 1. queue has space / 2. cancellation was requested
   cv.wait(lock, stopToken, [this] {
     return queue.size() < MAX_SIZE;
   });

   if (stopToken.stop_requested()){
    break;
   }

   queue.push(++p);
   std::cout << "pushed " << p << " to the queue!\n";

   lock.unlock();
   cv.notify_all();
  }//wh
 }//run
};

int main()
{
 BigIntegerProducer producer;

 std::jthread thread(
  [&producer](std::stop_token stopToken){
   producer.run(std::move(stopToken));
  });

 std::this_thread::sleep_for(std::chrono::milliseconds(100));
 std::cout << "Task stop requested!\n";
 thread.request_stop();

 return 0;
}