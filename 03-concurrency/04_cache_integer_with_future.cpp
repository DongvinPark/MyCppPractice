#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <future>
#include <iostream>
#include <mutex>
#include <numeric>
#include <random>
#include <thread>
#include <unordered_map>
#include <vector>

/*
cacheMap 안에 key 로는 1 ~ 100 의 정수를 담고, value 로는 key^2 값을 담는다.
이 작업을 10 개의 스레드에게 맡기고, mutex lock을 써서 한 번에 하나의 스레드만 맵에 접근 가능하게 만든다.

각 스레드는 1~100의 정수가 랜덤하게 섞인 리스트를 순회하면서 캐시를 조회한다.
특정 키에 대한 작업이 아직 등록되어 있지 않으면 해당 키에 대한 shared_future를 생성하여 캐시에 등록하고 계산 작업을 시작한다.
이미 등록된 경우에는 기존 shared_future를 가져와 해당 작업의 결과를 기다린다(중복 계산 방지용).

C++ 에서 비동기 작업을 정의/활용하기 위한 주요 도구인
std::pomise / std::future / std::shared_future 를 직접 다룬다.

///////////////////////

C++에서 std::promise와 std::future는 멀티스레딩 환경에서 비동기 작업(Asynchronous Task)의
결과 데이터를 안전하게 전달하고 동기화하기 위해 사용하는 핵심 동시성(Concurrency) 도구이다.

두 클래스는 항상 짝을 이루어 동작한다.

std::promise (생산자 측): 비동기 작업의 결과값을 설정(세팅)하는 역할이다. "나중에 이 값을 줄게"라고 약속하는 객체다.

std::future (소비자 측): 비동기 작업의 결과값을 받아오는(기다리는) 역할이다.
promise가 값을 줄 때까지 블로킹(대기)하거나, 값이 준비되었는지 확인할 수 있다.

set_value() / set_exception(): promise에서 사용한다.
작업이 성공했을 때는 결과값을, 예외가 발생했을 때는 예외 포인터를 설정하여 future 쪽으로 전달한다.

get_future(): promise 객체당 딱 한 번 호출할 수 있으며, 이 promise와 연결된 future를 반환한다.

get(): 결과를 반환한다. 결과가 아직 준비되지 않았다면 결과가 준비될 때까지 현재 스레드를 대기(block)시킨다.
한 번 호출하면 값을 소비하므로 두 번 이상 호출할 수 없다.

std::future는 단일 소비자를 위한 객체이므로 여러 스레드가 동일한 결과를 공유하기 어렵다.
.share()를 호출하면 std::shared_future로 변환되며,
여러 스레드가 동일한 shared state에 대해 get()을 호출하여 같은 결과를 읽을 수 있다.
*/

class FutureCache
{
private:
    std::unordered_map<int, std::shared_future<int>> cacheMap;
    std::mutex cacheMutex;

    static int expensiveCompute(int requestNumber)
    {
        int max = 10'000'000;
        while (max != 0){ --max; }
        return requestNumber * requestNumber;
    }

public:
    std::size_t cacheSize()
    {
        std::lock_guard<std::mutex> lock(cacheMutex);
        return cacheMap.size();
    }

    int compute(int requestNumber)
    {
        std::shared_future<int> future;
        {// 크리티컬 세션의 시작. 즉, 뮤텍스 락으로 보호 시작. 한 번에 하나의 스레드만 cacheMap에 액세스 할 수 있게 만든다.
            std::lock_guard<std::mutex> lock(cacheMutex); // 락 획득.

            auto it = cacheMap.find(requestNumber);
            if (it != cacheMap.end()) // 이미 등록해 놓은 작업이 있는 경우.
            {
                future = it->second;
            } else // 등록된 작업이 없는 경우. shared future 타입으로 '작업'을 만들어서 맵에 등록시킨다.
            {
                std::promise<int> promise;
                // 여기서 .share();를 호출해줘야 여러 스레드에서 future 의 결과(promise에서 초기화한 값)를 get() 호출로 받을 수 있다.
                future = promise.get_future().share();

                cacheMap.emplace(requestNumber, future);

                // expensive compute 작업을 스레드 detach()를 통해서 async 하게 시작시킨다.
                std::thread(
                    // C++ 람다는 기본적으로 캡쳐 및 전달 받은 것들을 const 취급한다. 아래의 lambda에서는
                    // 전달 받은 promise를 직접 수정해야 하기 때문에 mutable 키워드를 명시해줘야 한다.
                    [p = std::move(promise), requestNumber]() mutable
                    {
                        try
                        {
                            const int result = expensiveCompute(requestNumber);
                            p.set_value(result);
                        } catch (...)
                        {
                            p.set_exception(std::current_exception());
                        }
                    }
                    ).detach();
            }
        }// // 크리티컬 세션의 끝. cacheMutex가 여기를 벗어나 소멸하면서 소멸자에서 락을 해제한다.
        //std::lock_guard 가 RAII 객체이기 때문에 이러한 동작이 가능하다.

        // wait for result if necessary
        return future.get();
    }// end of compute()
};

class TaskImpl
{
private:
    FutureCache& cache;
    std::vector<int> numberList;
    std::mutex& startMutex;
    std::condition_variable& startCv;
    bool& start;
    int result = 0;

public:
    TaskImpl( // constructor
        FutureCache& cache,
        std::vector<int> numberList,
        std::mutex& startMutex,
        std::condition_variable& startCv,
        bool& start
        ) :
    cache(cache),
    numberList(std::move(numberList)),
    startMutex(startMutex),
    startCv(startCv),
    start(start) {}

    void operator()()
    {
        {
            std::unique_lock<std::mutex> lock(startMutex);
            startCv.wait(lock, [&]{return start;} );
        }

        for (int number : numberList)
        {
            result += cache.compute(number);
        }

        if (result != 338'350) // 338350 == 1^2 + 2^2 + ... + 100^2
        {
            std::cout << "Wrong result! : " << result << "\n";
        }
    }
};

int main()
{
    constexpr int NUMBER_OF_THREADS = 10;

    FutureCache futureCache;

    std::mutex startMutex;
    std::condition_variable startCv;
    bool start = false;

    std::vector<std::thread> threads;
    std::mt19937 rng(std::random_device{}());

    for (int i = 0; i < NUMBER_OF_THREADS; i++)
    {
        std::vector<int> numbers(100);
        std::iota(numbers.begin(), numbers.end(), 1);
        std::shuffle(numbers.begin(), numbers.end(), rng);

        threads.emplace_back(
            TaskImpl(
                futureCache,
                std::move(numbers),
                startMutex,
                startCv,
                start
                )
            );
    }//for

    auto begin = std::chrono::steady_clock::now();
    {
        std::lock_guard<std::mutex> lock(startMutex);
        start = true;
    }
    startCv.notify_all(); // 10 개 스레드들이 동시에 TaskImple을 시작하게 만든다.

    for (auto& thread : threads)
    {
        thread.join();
    }
    auto end = std::chrono::steady_clock::now();

    /// 결과 출력.
    auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            end - begin
        ).count();
    std::cout << "Elapsed time(ms) : " << elapsed << '\n';
    std::cout << "Cache Map size : " << futureCache.cacheSize() << '\n'; // expected value : 100
}//main















































