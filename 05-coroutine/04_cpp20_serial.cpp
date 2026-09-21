#include <coroutine>
#include <iostream>

/*
C++20에서는 std::generator가 없더라도 coroutine 자체는 사용할 수 있다.
(C++20 미만 버전에서는 당연히 코루틴 자체가 사용 불가능하다)
그러나, std::generator<T,T> 클래스가 해주는 일들을 개발자가 전부 직접 정확하게 구현해야 한다.
즉, coroutine의 결과를 어떻게 외부에 노출할지는 개발자가 결정(==구현)해야 한다.

(
실전에서 코루틴을 써야 한다면 코루틴 클래스를 직접 구현하기보다는,
C++ 버전을 23 으로 올린 후 std::lib 내의 코루틴 관련 클해스들을 테스트 해보거나
코루틴 객체 구현을 제공하는 라이브러리(boost 등)를 사용하는 것을 추천한다.
)

아래는 coroutine 및 iterator를 이용해서
std::generator와 비슷하게 range-based for를 사용할 수 있도록
간단하게 구현한 예제이며, Chat GPT를 활용했다.

C++20 표준은 coroutine을 구현하기 위한 일정한 Promise Type Protocol을 정의하고,
compiler는 그 protocol에 맞는 promise_type을 이용하여 coroutine state machine과
coroutine frame을 생성한다.

그래서 std::generator<T, T> 클래스를 대신할 SimpleSerialGenerator 를 직접 정의할 때,
initial_suspend
final_suspend
return_void
yield_void
unhandled_exception
get_return_object
등을 정의해줘야 하며, 이것들이 정의되지 않거나 생략되면 처음부터 컴파일조차 되지 않는다.

제어 흐름을 보면 아래와 같다.
count_to(5)
    │
    ▼
promise_type 생성
    │
    ▼
get_return_object()
    │
    ▼
SimpleSerialGenerator 반환
    │
    ▼
initial_suspend()
    │
    ▼
      [SUSPENDED]
    │
    │ handle.resume()
    ▼
for문 실행
    │
    ▼
co_yield 1
    │
    ▼
yield_value(1)
    │
    ▼
      [SUSPENDED]
    │
    │ handle.resume()
    ▼
co_yield 2
    │
    ▼
yield_value(2)
    │
    ▼
      [SUSPENDED]
    │
   ...
    │
    ▼
함수 종료
    │
    ▼
final_suspend()
    │
    ▼
      [SUSPENDED]
    │
    ▼
handle.destroy()
*/

class SimpleSerialGenerator {
public:
    struct promise_type {
        int current_value;

        // coroutine이 처음 생성됐을 때 바로 실행하지 않음
        std::suspend_always initial_suspend() { return {}; }
        // coroutine이 끝나면 suspend
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() { std::exit(-1); }

        // co_yield value
        std::suspend_always yield_value(int value) {
            current_value = value;
            return {};
        }

        SimpleSerialGenerator get_return_object() {
            return SimpleSerialGenerator{
                std::coroutine_handle<promise_type>::from_promise(*this)
            };
        }
    };//end of internal promise_type for coroutine class.

private:
    std::coroutine_handle<promise_type> handle;

public:
    explicit SimpleSerialGenerator(std::coroutine_handle<promise_type> handle)
        : handle(handle) {}

    ~SimpleSerialGenerator() {
        if (handle) {
            handle.destroy();
        }
    }

    // 복사 방지
    SimpleSerialGenerator(const SimpleSerialGenerator&) = delete;
    SimpleSerialGenerator& operator=(const SimpleSerialGenerator&) = delete;
    // move 허용
    SimpleSerialGenerator(SimpleSerialGenerator&& other) noexcept
        : handle(other.handle) {
        other.handle = nullptr;
    }

    // 다음 값을 얻을 수 있는 상태인가?
    bool next() {
        if (!handle || handle.done()) {
            return false;
        }
        handle.resume();
        return !handle.done();
    }

    // 현재 값을 얻는다.
    int value() const {
        return handle.promise().current_value;
    }


    // =========================================================
    // Iterator
    // =========================================================
    class Iterator {
    private:
        SimpleSerialGenerator* generator;
        bool finished;

    public:
        Iterator(SimpleSerialGenerator* generator, bool finished)
            : generator(generator),
              finished(finished) {}

        // *iterator : 값 리턴.
        int operator*() const {
            return generator->value();
        }

        // ++iterator
        Iterator& operator++() {
            finished = !generator->next();
            return *this;
        }

        // iterator != end
        bool operator!=(const Iterator& other) const {
            return finished != other.finished;
        }
    };


    // range-for의 시작점
    Iterator begin() {
        // 첫 번째 값을 만들어 놓는다.
        bool finished = !next();

        return Iterator(this, finished);
    }

    // range-for의 끝
    Iterator end() {
        return Iterator(this, true);
    }
};// end of SimpleSerialGenerator


// =========================================================
// Coroutine 객체 리턴 함수 정의.
// =========================================================
SimpleSerialGenerator count_to(int n) {
    for (int i = 1; i <= n; ++i) {
        co_yield i;
    }
}


// =========================================================
// Main
// =========================================================
int main() {
    for (int v : count_to(5)) {
        std::cout << v << " ";
    }

    std::cout << "\n";
    return 0;
}