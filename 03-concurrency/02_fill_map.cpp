#include <iostream>
#include <mutex>
#include <chrono>
#include <unordered_map>

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>

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









































