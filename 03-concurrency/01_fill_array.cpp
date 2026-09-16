#include<iostream>
#include<stdexcept>
#include<thread>
#include<vector>
#include<latch>
#include<chrono>
#include<cstddef>

// 길이가 2 억 인 배열을 채우는 예제.
// C++ 20 이상에서 실행 가능.

/* M1 Ulata chip Mac Studio 에서 실행해보면 아래이 거의 10 배 가까운 성능 향상을 확인할 수 있다.
이건 사실 코드가 좋아서라기보다는 M1 Ulatachip 이 실제로 엄청난 멀티코어 성능을 제공하기 때문이기는 하다.
➜  03-concurrency git:(master) ✗ pwd
/Users/dongvin99/Documents/MyCppPractice/03-concurrency
➜  03-concurrency git:(master) ✗ g++ -std=c++20 -o executable 01_fill_array.cpp && ./executable && rm executable
One main thread time(ms): 263 ms
arr1 last value: 199999999
20 Multi thread time(ms): 27 ms
arr2 last value: 199999999
 */

class FillArrayByMultiThread {
private:
  static void printElapsed(
      const std::string& label,
      std::chrono::steady_clock::time_point start,
      std::chrono::steady_clock::time_point end
  ){
    const auto elapsed
        = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << label << ": " << elapsed << " ms" << std::endl;
  }

  public:
    void run(){
      constexpr std::size_t N = 200'000'000;
      constexpr std::size_t CHUNK = 10'000'000;
      constexpr std::size_t THREAD_COUNT = N / CHUNK;

      int* arr1 = new int[N];
      int* arr2 = new int[N];

      ///////// 1 개의 메인 스레드에서 배열 채우고 시간 측정.
      auto start = std::chrono::steady_clock::now();
      for(std::size_t i = 0; i < N; ++i){
        arr1[i] = static_cast<int>(i);
      }
      auto end = std::chrono::steady_clock::now();
      printElapsed("One main thread time(ms)", start, end);
      std::cout << "arr1 last value: " << arr1[N-1] << '\n';

      ///////// 멀티 스레드(20개)로 배열 채우기 시간 측정. 인덱스가 서로 겹치지 않으므로 뮤텍스 락은 쓰지 않는다.

      // 카운트 다운 랫치 생성 : 스레드들의 직업이 동시에 시작 되도록.
      std::latch startGate(1);
      std::latch endGate(
          static_cast<std::ptrdiff_t>(THREAD_COUNT)
      );

      // 스레드 20개 생성 + task lambda 정의.
      std::vector<std::thread> workers;
      workers.reserve(THREAD_COUNT);

      for(std::size_t startIdx = 0; startIdx < N; startIdx += CHUNK){
        const std::size_t endIdx = startIdx + CHUNK;

        workers.emplace_back(
            // lambda 정의. 외부 array를 조작해야 하기 때문에 catch mode를 '참조'로 설정해야 한다.
            [&, startIdx, endIdx](){
              startGate.wait(); // 직업 시작 될 때까지 대기.
              for(std::size_t i = startIdx; i < endIdx; i++){
                arr2[i] = static_cast<int>(i);
              }
              endGate.count_down(); // 작업 마친 후, 마쳤다는 사실 알림.
            }
        );
      }

      // 시간 측정.
      start = std::chrono::steady_clock::now();
      startGate.count_down();
      endGate.wait();
      end = std::chrono::steady_clock::now();
      printElapsed("20 Multi thread time(ms)", start, end);
      std::cout << "arr2 last value: " << arr2[N-1] << '\n';

      // 워커 스레드 조인
      for(auto& worker : workers){
        worker.join();
      }

      // 메모리 회수
      delete[] arr1;
      delete[] arr2;
    }
};


int main (){
  FillArrayByMultiThread fillArray;
  fillArray.run();
}


























