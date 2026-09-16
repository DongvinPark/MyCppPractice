#include <iostream>
#include <chrono>
#include <unordered_map>
#include <array>

#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>

/*
이번엔 02_fill_map.cpp 에서 mutex 없이도 작동하도록 수정해보았다.

Asus Rog Strix Gaming laptop에서 테스트 했을 때,
single thread에서는 약 6.7 초가 걸렸고,
멀티 스레드(하지만 각 스레드는 독립된 맵에 접근)에서는 약 9초가 걸렸다.

02_fill_map.cpp에서 보여준 싱글 vs 멀티 보다는 성능 격차가 크게 줄었는데,
mutex lock/unlock 동작이 없어졌기 때문이다.

그래도 교훈은 명확하다. 똑같은 일을 싱글 스레드로 처리할 수 있으면 굳이 멀티 스레드로
억지로 확장해서 처리할 필요가 없으며,
mutex 락을 Data Structure 전역에 사용하는 것은 multi threaded 환경에서는 매우 큰
Read/Write 비용을 발생시킨다는 점이다.
 */

class FillMapByThreadPool {
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
        std::array<std::unordered_map<int, int>, THREAD_CNT> localMaps;
        boost::asio::thread_pool threadPool(THREAD_CNT);

        auto multiStart = std::chrono::steady_clock::now();
        for (int startIdx = 0; startIdx < TOTAL; startIdx += CHUNK)
        {
            int endIdx = startIdx + CHUNK -1;
            boost::asio::post(
                threadPool,
                [&, startIdx, endIdx]() {
                    const int mapIdx = startIdx / CHUNK;
                    auto& partialMap = localMaps[mapIdx];
                    for (int i = startIdx; i <= endIdx; ++i) {
                        partialMap.emplace(i, i);
                    }
                }
                );
        }//thread for
        threadPool.join();

        auto multiEnd = std::chrono::steady_clock::now();

        std::cout << THREAD_CNT << " threads execution time(ms) : "
                  << std::chrono::duration_cast<
                         std::chrono::milliseconds>(
                             multiEnd - multiStart).count() << " \n";

        int localMapSize = 0;
        for (int i = 0; i < THREAD_CNT; ++i) {
            auto& partialMap = localMaps[i];
            localMapSize += static_cast<int>(partialMap.size());
        }
        std::cout << "ConcurrentMap size: " << localMapSize << '\n';
    }
};


int main()
{
    FillMapByThreadPool threadPool{};
    threadPool.run();
}