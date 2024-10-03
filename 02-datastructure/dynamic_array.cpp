#include<iostream>
#include<stdexcept>
#include<utility>

using namespace std;

class MyArray {
    private:
        int* data;
        size_t size;

    public:
        // Constructor : 객체를 새로 만들어 낼 때 호출된다.
        MyArray(size_t n) : size(n) {
            data = new int[size];
            cout << "Constructor: Allocation " << size << " elements\n";
        }

        // Destructor : 객체가 소멸될 때 호출된다. 자원해제에 필수적이다.
        ~MyArray() {
            delete[] data;
            cout << "Destructor: Releasing memory\n";
        }

        // Copy Constructor : 새로운 객체를 선언함과 동시에 이전에 만들어둔 객체로 초기화 할 때 호출된다.
        // MyArray arr2 = other_arr_made_before; 이런 코드다.
        MyArray(const MyArray& other) : size(other.size) {
           data = new int[size];
           std::copy(other.data, other.data + size, data);
           cout << "Copy Constructor: Copying " << size << " elements\n";
        }

        // Copy Assignment Operatori : 이미 만들어져 있는 객체를 또 다른 이미 만들어져 있는 객체로 초기화
        // 할 때 호출된다.
        // MyArray arr3(10); arr3 = other_arr_made_before; 이런 코드다.
        MyArray& operator=(const MyArray& other){
            if(this != &other){
                delete[] data;

                size = other.size;
                data = new int[size];
                std::copy(other.data, other.data + size, data);
                cout << "Copy Assignment: Copying " << size << " elements\n";
            }
            return *this;
        }

        // Move Constructor
        MyArray(MyArray&& other) noexcept : data(other.data), size(other.size) {
            other.data = nullptr;
            other.size = 0;
            cout << "Move Constructor: Moving " << size << " elements\n";
        }

        // Move Assignment Operator
        MyArray& operator=(MyArray&& other) noexcept {
            if(this != &other){
                delete[] data;

                data = other.data;
                size = other.size;

                other.data = nullptr;
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
};// end of MyArray. C++ 에서는 클래스 정의가 끝나는 중괄호에 ; 를 꼭 붙여야 한다!!

MyArray createTemporaryArray(size_t n){
    // 원래대로라면, 지역변수는 자기 범위를 벗어나면 소멸한다.
    // 그러나 함수의 결과값이 소멸돼서는 안 되므로, 이동 생성자 또는 이동 연잔자를 통해서
    // 지역 변수가 새로운 범위로 '이동'할 수 있게 해줘야 한다. 이때 쓸데 없는 복사가 일어나지 않게
    // 해주는 것이 중요하다.
    MyArray tempArray(n);
    return tempArray;
}

int main() {

    // 생성하는 것들(생성자, 복사생성자, 이동생성자)들은 할당(==객체생성)과 초기화가 ';'로 끝나는
    // 하나의 문장에서 전부 이루어지는 반면,
    // 이동시키는 것들(복사 연산자, 이동 연산자)은 할당과 초기화가 별개의 ';'로 분리돼 있다는 
    // 중요한 차이점이 있다.
    // 또한, 이동 생성자와 이동 연산자는 원래대로라면 지역 범위를 벗어나서 소멸해야 할 지역 변수를
    // MyArray arr = createTemporaryArray(10); 과 같이 지역 범위를 벗어날 수 있게 해주는 역할을 한다.

    MyArray arr1(5); // Constructor
    arr1[0] = 10; // Access element
    arr1[1] = 20;

    MyArray arr2 = arr1; // Copy constructor

    MyArray arr3(10); // Constructor
    arr3 = arr1; // Copy Assignment operator
    
    cout << "\n worked well. let's see move sementics \n\n";

    // 원래대로라면, 여기에서 move constructor가 호출돼야 하지만, 컴파일러의 최적화 기법인,
    // copy elision, RVO(==return value optimization) 등이 작동해서 호출되지 않는다.
    MyArray arr4 = createTemporaryArray(7); // Move Constructor (from temporary)

    MyArray arr5(3); // Constructor
    arr5 = createTemporaryArray(7); // Move Assignment opeartor
   
    cout << "\n Destructors will be called !! \n\n";
    // All Destructors will be called automatically at the end
    return 0;
}






























