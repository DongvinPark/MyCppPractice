#include <mutex>
#include <deque>
#include <iostream>
#include <thread>
#include <random>
#include <latch>
#include <chrono>

/*
10 개의 프로듀서 스레드들이 각각 1000 개의 작업을 BlockingQueue에 집어 넣으며,
10개의 컨슈머 스레드들이 BlockingQueue의 작업들을 꺼내서 처리한다.

20 개의 스레드는 startGate std::latch에 의해서 거의 동시에 각자의 작업을 시작한다.

작업 한 개를 처리하는 것에 걸리는 시간은 1밀리초 ~ 10밀리초 이고, 이는 작업을 처리하기 시작한 시점에
ThreadLocalRandom 난수에 의해서 결정된다.

컨슈머 스레드들을 자기가 가져온 작업을 처리하기만 할 뿐, 다른 컨슈머 스레드들에게는 관여하지 않는다.

06_blocking_queue_mutex_only.cpp 버전과 달리, condition_variable 을 사용한다.
따라서 큐가 비어 있을 때 poll()을 호출한 스레드는 큐에 뭔가 채워질때까지 기다릴 수 있게 되며,
00 mutex only 버전과 달리 '너무 일찍 시작한 컨슈머 스레드들'이 빨리 종료되버리는 문제를 해결한다.
*/

constexpr int THREAD_COUNT = 10;
constexpr int TASK_COUNT = 1000;
constexpr int CONSUMER_THREAD_START_DELAY_MS = 10;

class Task {
  public:
  bool isValid;

  void work(){
    try {
      // java 의 int timeToNeed = ThreadLocalRandom.current().nextInt(1,11); 의 C++ 버전.
      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_int_distribution<int> dist(1, THREAD_COUNT);
      int timeToNeed = dist(gen);

      std::this_thread::sleep_for(std::chrono::milliseconds(timeToNeed));
    } catch(...){
      std::cerr << "Exception thrown while work!\n";
    }
  }
};

////////////////////////////////////
class BlockingQueue {
private:
  std::mutex m;
  std::condition_variable cv;
  std::deque<Task> data;

public:
  int size(){
    std::lock_guard<std::mutex> lock(m);
    return static_cast<int>(data.size());
  }

  bool is_empty(){
    std::lock_guard<std::mutex> lock(m);
    return data.empty();
  }

  void put(const Task& task){
    {
      std::lock_guard<std::mutex> lock(m);
      data.push_back(std::move(task));
    }

    cv.notify_one();
  }

  std::unique_ptr<Task> poll(){
    std::unique_lock<std::mutex> lock(m);

    // 여기를 통해서 poll() 호출한 스레드가 '큐에 뭔가 들어올 때까지' 블록킹될 수 있다.
    cv.wait(lock, [this]() {
      return !data.empty();
    });

    if (data.empty()){
      return nullptr;
    }

    auto task = std::make_unique<Task>(std::move(data.front()));

    data.pop_front();
    return task;
  }
};//end of Blocking Queue
///////////////////////////////////////

int main(){
  BlockingQueue blockingQueue;

  std::latch producerStartGate(1);
  std::latch startGate(1);
  std::latch endGate(THREAD_COUNT);
  std::mutex coutMutex;

  // 10 개의 스레드들이 작업을 1000 개 만드는 스레드 생성.
  std::vector<std::thread> producerWorkers;
for(int i = 0; i < THREAD_COUNT; i++){
    producerWorkers.emplace_back(
          [&blockingQueue, &producerStartGate](){
          producerStartGate.wait();
          for(int i = 0; i < TASK_COUNT; i++){
            blockingQueue.put(Task{true});
          }
        }
    );
}//for

  // consumer thread 10개 생성.
  std::vector<std::thread> consumerWorkers;
for(int i = 0; i < THREAD_COUNT; i++){
    consumerWorkers.emplace_back(
    [&blockingQueue, &startGate, &endGate, &coutMutex](){
      try {
        int taskDoneCnt = 0;
        auto start = std::chrono::high_resolution_clock::now();
        startGate.wait();

        while(true){
          auto taskPtr = blockingQueue.poll();
          if(taskPtr != nullptr){
            taskPtr->work();
            taskDoneCnt++;
          }
          if (taskDoneCnt >= TASK_COUNT){break;}
        }//wh

        auto end = std::chrono::high_resolution_clock::now();

        {
          // coutMutex를 넣지 않으면 로깅이 엉망으로 꼬인다.
          // std::cout 이 thread-safe 하지 않아서 그렇다.
          std::lock_guard<std::mutex> lock(coutMutex);
          std::cout << "task processing time(ms) : "
             << std::chrono::duration_cast<
               std::chrono::milliseconds
              >(end - start).count()
                << " / thread id : " << std::this_thread::get_id()
                << " / task done cnt " << taskDoneCnt << "\n";
        }
      } catch(...){
        std::cerr << "Exception thrown in worker thread! thread id : "
                  << std::this_thread::get_id() << "\n";
      }
      endGate.count_down();
    }//thread lambda
    );
  }//for

  // 필요하다면 아래의 주석을 해제해서 컨슈머 스레드들을 일부러 늦게 실행시킬 수도 있다.
  producerStartGate.count_down();
  /*std::this_thread::sleep_for(
    std::chrono::milliseconds(CONSUMER_THREAD_START_DELAY_MS)
  );*/

  const auto start = std::chrono::steady_clock::now();
  startGate.count_down(); // producer 들과 consumer 들이 동시에 작업 시작.
  endGate.wait();
  const auto end = std::chrono::steady_clock::now();

  std::cout << "Execution time(ms) : "
            << std::chrono::duration_cast<
                  std::chrono::milliseconds
                >(end - start).count() << "\n";

  // worker 스레드들을 막판에 조인시켜 주지 않으면 SIGABART 에러가 뜬다.
  for(auto& t : producerWorkers){
    t.join();
  }
  for(auto& t : consumerWorkers){
    t.join();
  }
  return 0;
}












































