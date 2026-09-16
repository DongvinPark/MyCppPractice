#include <iostream>
#include <mutex>
#include <chrono>
#include <unordered_map>

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>

/*
map을 스레드 1개일 때와 10일 때 각각 채워보고 그 결과를 기록한 것.
map 안에 여러 스레드들이 요소 하나를 넣으려고 할 때마다 mutex 락을 걸고 푸는 동작을 하느라
시간이 싱글 스레드로 처리하는 것 보다 2~4 배 정도가 걸린다.

01_fill_array.cpp 의 경우에도 여러 스레드들이 하나의 배열에 접근을 하지만, 이때는
각 스레드 별로 접근 가능 인덱스를 철저하게 분리했기 때문에 mutex lock/unlock 이 없었다.
그 결과 스레드가 늘어나는 만큼 성능이 향상되었다.
 */

class FillMapByThreadPool
{
public:
    void run()
    {
        constexpr int TOTAL = 10'000'000;
        constexpr int CHUNK = 1'000'000;
        constexpr int THREAD_CNT = TOTAL / CHUNK;

        // 싱글 스레드로 채우는 시간 측정.
        std::unordered_map<int, int> commonMap;
        auto start = std::chrono::steady_clock::now();
        for (int i = 0; i < TOTAL; ++i)
        {
            commonMap.emplace(i, i);
        }
        auto end = std::chrono::steady_clock::now();

        std::cout << "1 thread execution time(ms) : "
                  << std::chrono::duration_cast<
                         std::chrono::milliseconds>(end - start).count() << "\n";
        std::cout << "CommonMap size: " << commonMap.size() << '\n';

        // 멀티 스레드로 채우는 시간 측정.
        std::unordered_map<int, int> concurrentMap;
        std::mutex mutex;

        boost::asio::thread_pool threadPool(THREAD_CNT);

        auto multiStart = std::chrono::steady_clock::now();
        for (int startIdx = 0; startIdx < TOTAL; startIdx += CHUNK)
        {
            int endIdx = startIdx + CHUNK -1;
            boost::asio::post(
                threadPool, [startIdx, endIdx, &concurrentMap, &mutex]()
                {
                    for (int i = startIdx; i <= endIdx; ++i)
                    {
                        std::lock_guard<std::mutex> lock(mutex);
                        concurrentMap.emplace(i, i);
                    }
                }
                );
        }//thread for
        threadPool.join();
        auto multiEnd = std::chrono::steady_clock::now();

        std::cout << THREAD_CNT << " threads execution time: "
                  << std::chrono::duration_cast<
                         std::chrono::milliseconds>(
                             multiEnd - multiStart).count()
                  << " ms\n";

        std::cout << "ConcurrentMap size: " << concurrentMap.size() << '\n';
    }
};


int main()
{
    FillMapByThreadPool threadPool{};
    threadPool.run();
}









































