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

컨슈머 스레드들을 자기가 가져온 작업을 처리하기만 할 뿐, 다른 컨슈머 스레드들의 작업 처리에는 관여하지 않는다.

M1 Ultra chip Mac Studio에서 실행해보면 아래와 같은 로깅이 나온다.

/Users/dongvin99/Documents/MyCppPractice/cmake-build-debug/MyCppPractice
task processing time(ms) : 0 / thread id : 0x16bf3f000 / task done cnt 0
task processing time(ms) : 7346 / thread id : 0x16beb3000 / task done cnt 1097
task processing time(ms) : 7347 / thread id : 0x16bbf7000 / task done cnt 1099
task processing time(ms) : 7348 / thread id : 0x16bb6b000 / task done cnt 1087
task processing time(ms) : 7348 / thread id : 0x16ba53000 / task done cnt 1115
task processing time(ms) : 7348 / thread id : 0x16bd0f000 / task done cnt 1135
task processing time(ms) : 7348 / thread id : 0x16bd9b000 / task done cnt 1117
task processing time(ms) : 7350 / thread id : 0x16be27000 / task done cnt 1122
task processing time(ms) : 7353 / thread id : 0x16badf000 / task done cnt 1123
task processing time(ms) : 7355 / thread id : 0x16bc83000 / task done cnt 1105
Execution time(ms) : 7356

condition variable 이 없어서, consumer thread 들 중 첫 번째로 시작 작업을 시작한 스레드는
Blocking Queue 가 비어 있는 것을 확인한 후 곧장 종료돼 버린다.

queue 가 비어 있을 경우 while(){...} 루프를 곧장 빠져나와 버리기 때문이다.
스레드 종료 후에는 남은 Task들을 처리하지 않게 되므로, 결과적으로 성능을 깎아 먹게 된다.
*/

class Task {
  public:
  bool isValid = true;

  void work(){
    try {
      // java 의 int timeToNeed = ThreadLocalRandom.current().nextInt(1,11); 의 C++ 버전.
      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_int_distribution<int> dist(1, 10);
      int timeToNeed = dist(gen);

      std::this_thread::sleep_for(std::chrono::milliseconds(timeToNeed));
    } catch(...){
      std::cerr << "Exception thrown while work!\n";
    }
  }
};

class BlockingQueue {
private:
  std::mutex m;
  std::deque<Task> data;

public:
  int size(){
    std::lock_guard<std::mutex> lock(m);
    return data.size();
  }

  bool is_empty(){
    std::lock_guard<std::mutex> lock(m);
    return data.empty();
  }

  void put(){
    std::lock_guard<std::mutex> lock(m);
    data.push_back(Task{});
  }

  std::unique_ptr<Task> poll(){
    std::unique_lock<std::mutex> lock(m);
    if (data.empty()){
      return nullptr;
    }

    //auto& task = data.front(); 이건 잘못된 코드다. dangling ptr을 리턴하게 된다.

    auto task = std::make_unique<Task>(std::move(data.front()));

    data.pop_front();
    return task;
  }
};

int main(){
  BlockingQueue blockingQueue;

  std::latch startGate(1);
  std::latch endGate(10);

  std::mutex coutMutex;

  // 10 개의 스레드들이 작업을 1000 개 만드는 스레드 생성.
  std::vector<std::thread> producerWorkers;
for(int i = 0; i < 10; i++){
    producerWorkers.emplace_back(
          [&blockingQueue, &startGate](){
          startGate.wait();
          for(int i = 0; i < 1000; i++){
            blockingQueue.put();
          }
        }
    );
  }//for

  // consumer thread 10개 생성.
  std::vector<std::thread> consumerWorkers;
for(int i = 0; i < 10; i++){
    consumerWorkers.emplace_back(
    [&blockingQueue, &startGate, &endGate, &coutMutex](){
      try {
        int taskDoneCnt = 0;
        auto start = std::chrono::high_resolution_clock::now();

        startGate.wait();
        while(!blockingQueue.is_empty()){
          auto taskPtr = blockingQueue.poll();
          if(taskPtr != nullptr){
            taskPtr->work();
            taskDoneCnt++;
          }
        }//wh
        auto end = std::chrono::high_resolution_clock::now();

        {
          // coutMutex를 넣지 않으면 로깅이 엉망으로 꼬인다. std::cout 이 thread-safe 하지 않아서 그렇다.
          std::lock_guard<std::mutex> lock(coutMutex);
          std::cout << "task processing time(ms) : "
             << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
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

  auto start = std::chrono::steady_clock::now();
  startGate.count_down(); // producer 들과 consumer 들이 동시에 작업 시작.
  endGate.wait();
  auto end = std::chrono::steady_clock::now();

  std::cout << "Execution time(ms) : "
            << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "\n";

  // worker 스레드들을 막판에 조인시켜 주지 않으면 SIGABART 에러가 뜬다.
  for(auto& t : producerWorkers){
    t.join();
  }
  for(auto& t : consumerWorkers){
    t.join();
  }
  return 0;
}












































