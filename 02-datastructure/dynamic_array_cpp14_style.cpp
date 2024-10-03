#include<iostream>
#include<stdexcept>
#include<utility>
#include<memory>
#include<algorithm>

using namespace std;

class MyArray {
    private:
        unique_ptr<int[]> data;
        size_t size;

    public:
        // Constructor
        MyArray(size_t n) : size(n), data(make_unique<int[]>(n)) {
            cout << "Constructor: Allocating " << size << " elements\n";
        }

        // Destructor
        // std::unique_ptr가 자동으로 자원 해제를 담당하기 때문에 별도로 소멸자를 정의할 필요 없다.
        //~MyArray() = default;
        // 또는 동작을 확인하기 위한 소멸자를 정의하되, 그 안에서 실제로 자원 해제 동작을 해줄 필요가 없다.
        ~MyArray(){
            cout << "Destructor: Realeasing memory \n";
        }

        // Copy Constructor
        MyArray(const MyArray& other) : size(other.size), data(make_unique<int[]>(other.size)) {
            std::copy(other.data.get(), other.data.get()+size, data.get());
            cout << "Copy Constructor: Copying " << size << " elements\n";
        }

        // Copy Assignment Operator
        MyArray& operator=(const MyArray& other){
            if (this != &other) {
                size = other.size;
                data = make_unique<int[]>(size);
                copy(other.data.get(), other.data.get() + size, data.get());
                cout << "Copy Assignment: Copying " << size << " elements\n";
            }
            return *this;
        }

        // Move Constructor
        MyArray(MyArray&& other) noexcept : size(other.size), data(std::move(other.data)) {
            other.size = 0;
            cout << "Move Constructor: Moving " << size << " elements\n";
        }

        // Move Assignment Operator
        MyArray& operator=(MyArray&& other) {
            if(this != &other){
                size = other.size;
                data = std::move(other.data);
                other.size = 0;
                cout << "Move Assignment: Moving " << size << " elements\n";
            }
            return *this;
        }

        // Element Access Function
        int& operator[](int idx){
            if(idx < 0 || idx >= size){
                throw std::out_of_range("Array index out of range!");
            }
            return data[idx];
        }
};

int main() {
    MyArray arr1(5); // Constructor
    arr1[0] = 10;    // Access element
    arr1[1] = 20;

    MyArray arr2 = arr1; // Copy constructor

    MyArray arr3(10); // Constructor
    arr3 = arr1;      // Copy Assignment operator
    
    // 컴파일러에서 eliding 및 Return Value Optimization 을 해서 이동 생성자는 실제로는 호출되지 않고,
    // arr4에 곧장 그냥 Constructor가 호출된 결과가 할당된다.
    MyArray arr4 = MyArray(7); // Move Constructor (from temporary) 

    MyArray arr5(3); // Constructor
    arr5 = MyArray(7); // Move Assignment operator

    return 0;
}
