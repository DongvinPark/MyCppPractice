
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <utility>

class MyArray {
private:
    int* data;
    std::size_t size;

public:
    // Constructor
    // 객체가 생성될 때 호출된다.
    // explicit 키워드 : 단일 인자를 받는 생성자를 통한 암묵적 변환을 방지한다.
    // 예: MyArray arr = 7; 같은 초기화를 막는다.
    explicit MyArray(std::size_t n)
        : data(new int[n]), size(n) {
        std::cout << "Constructor: Allocation "
                  << size << " elements\n";
    }

    // Destructor
    // 객체의 수명이 끝날 때 호출된다.
    // new[]로 할당한 메모리를 delete[]로 해제한다.
    ~MyArray() {
        delete[] data;
        std::cout << "Destructor: Releasing memory\n";
    }

    // Copy Constructor
    // 새로운 객체를 기존 객체로 초기화할 때 호출된다.
    // 깊은 복사(deep copy):
    // 원본 배열의 원소를 새로운 메모리에 복사한다.
    // 두 객체가 서로 다른 배열을 소유하므로,
    // 한 객체의 소멸이 다른 객체의 메모리에 영향을 주지 않는다.
    MyArray(const MyArray& other)
        : data(new int[other.size]), size(other.size) {
        std::copy(other.data,
                  other.data + size,
                  data);

        std::cout << "Copy Constructor: Copying "
                  << size << " elements\n";
    }

    // Copy Assignment Operator
    // 이미 존재하는 객체에 다른 객체의 값을 대입할 때 호출된다.
    // copy-and-swap:
    // 1. 복사 생성자로 임시 복사본을 만든다.
    // 2. swap을 통해 현재 객체와 자원을 교환한다.
    // 3. 함수가 끝나면 임시 객체(즉, other)가 기존 자원을 해제한다.
    // 복사 과정에서 예외가 발생해도 현재 객체는 유지된다.
    MyArray& operator=(const MyArray& other) {
        if (this != &other) {
            MyArray temp(other);
            swap(temp);
        }

        std::cout << "Copy Assignment: Copying "
                  << size << " elements\n";

        return *this;
    }

    // Move Constructor
    // 새로운 객체를 rvalue 객체로 초기화할 때 호출된다.
    // 배열 원소를 하나씩 복사하지 않고,
    // 기존 객체가 소유하던 메모리 포인터를 가져온다.
    // 원본 객체는 자원을 잃은 유효한 상태가 된다.
    MyArray(MyArray&& other) noexcept
        : data(other.data), size(other.size) {
        // 참고로 lvalue 객체는 메모리 상에 구체적인 주소를 가지고 저장되는 객체이고,
        // rvalue 객체는 주소를 가진 채 메모리에 저장되지 못하는 객체이다.
        other.data = nullptr;
        other.size = 0;

        std::cout << "Move Constructor: Moving "
                  << size << " elements\n";
    }

    // Move Assignment Operator
    // 이미 존재하는 객체에 rvalue 객체를 대입할 때 호출된다.
    MyArray& operator=(MyArray&& other) noexcept {
        if (this != &other) {
            delete[] data;

            data = other.data;
            size = other.size;

            other.data = nullptr;
            other.size = 0;
        }

        std::cout << "Move Assignment: Moving "
                  << size << " elements\n";

        return *this;
    }

    // Element Access
    // 호출자가 현재 객체를 수정 할 수 있을 때, 여기가 호출된다.
    // size_t를 사용하므로 음수 인덱스 검사는 필요하지 않다.
    // idx >= size이면 범위를 벗어난 접근이다.
    int& operator[](std::size_t idx) {
        if (idx >= size) {
            throw std::out_of_range(
                "Array index out of range!"
            );
        }

        return data[idx];
    }

    // Const Element Access
    // const 객체에서도 원소를 읽을 수 있도록 한다.
    // 즉, 호출자가 현재 객체를 수정 할 수 없을 때, 여기가 호출된다.
    const int& operator[](std::size_t idx) const {
        if (idx >= size) {
            throw std::out_of_range(
                "Array index out of range!"
            );
        }

        return data[idx];
    }

private:
    // 두 객체의 내부 자원을 교환한다.
    // 여기는 당연히 파라미터가 const 참조여서는 안 된다.
    void swap(MyArray& other) noexcept {
        std::swap(data, other.data);
        std::swap(size, other.size);
    }
};

// 반환값은 값으로 반환된다.
// 지역 객체의 수명이 연장되는 것이 아니라,
// 반환 객체가 복사 생략 또는 이동을 통해 만들어진다.
MyArray createTemporaryArray(std::size_t n) {
    MyArray tempArray(n);
    return tempArray;
}

int main() {
    MyArray arr1(5); // Constructor
    arr1[0] = 10;
    arr1[1] = 20;

    MyArray arr2 = arr1; // Copy Constructor

    MyArray arr3(10); // Constructor
    arr3 = arr1;      // Copy Assignment Operator

    std::cout << "\nWorked well. "
                 "Let's see move semantics.\n\n";

    // 반환값 초기화.
    // copy elision이 적용되면 이동 생성자 호출이 생략될 수 있다.
    MyArray arr4 = createTemporaryArray(7);

    MyArray arr5(3); // Constructor

    // 이미 존재하는 arr5에 임시 객체를 대입한다.
    // Move Assignment Operator가 호출될 수 있다.
    arr5 = createTemporaryArray(7);

    std::cout << "\nDestructors will be called!\n\n";

    return 0;
}