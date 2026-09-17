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
    std::unordered_map<int, std::shared_future<int>>& getCacheMap()
    {
        return cacheMap;
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
                // 여기서 .share();를 호출해줘야 int compute(){...} 의 호출자가 future 의 결과(promise에서 초기화한 값)를 공유 받을 수 있다.
                future = promise.get_future().share();

                cacheMap.emplace(requestNumber, future);

                // expensive compute 작업을 스레드 detach()를 통해서 async 하게 시작시킨다.
                std::thread(
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
    std::cout << "Cache Map size : " << futureCache.getCacheMap().size() << '\n'; // expected value : 100
}//main















































