#include <iostream>
#include <future>
#include <random>
#include <vector>

/*
10 개의 스레드 또는 async task 를 이용해서 랜덤 정수를 vector에 집어 넣는다.
홀수가 나올 경우 예외를 던지며, 예외를 던지 스레드의 아이디를 별도의 vertor에 기록한다.

그리고 std::async 의 사용방법을 설명한다.
 */

std::mutex mutex;

int threadWork(std::vector<std::thread::id>& exception_thread_ids)
{
    int randomInteger = (std::random_device{}() % 100) + 1;

    if (randomInteger % 2 == 0)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return randomInteger;
    }

    {
        std::lock_guard<std::mutex> lock(mutex);
        exception_thread_ids.push_back(std::this_thread::get_id());
    }
    throw std::runtime_error("Thread doesn't have an even number!");
}

int main()
{
    std::vector<std::future<int>> futures;
    std::vector<std::thread::id> exception_thread_ids;

    // 10 개의 비동기 작업
    for (int i = 0; i < 10; i++)
    {
        futures.push_back(
        std::async(
            std::launch::async, // hoo to launch
            threadWork,   // what function?
            std::ref(exception_thread_ids) // arguments for function. used std::ref to pass the vectors reference
            /* threadWork 가 더 많은 arguments 를 필요로 한다면 아래와 같은 방식으로 이어서 쓰면 된다.
            * std::async(
                launch policy,
                callable(== function's name),
                argument 1,
                argument 2,
                argument 3,
                ...
                )
            */
            )
        );
    }

    // 결과 확인
    for (auto& f : futures)
    {
        try
        {
            int result = f.get();
            std::cout << "Success : " << result << "\n";
        } catch (std::runtime_error& e)
        {
            std::cout << "\tException occurred while calling future.get()! : " << e.what() << "\n";
        }
    }

    // 예외 뜬 스레드들 아이디 확인
    std::cout << "\n\nexception thrown thread ids\n";
    for (auto& id : exception_thread_ids)
    {
        std::cout << id << "\n";
    }
}