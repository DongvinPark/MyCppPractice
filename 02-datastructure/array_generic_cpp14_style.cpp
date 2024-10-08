#include<iostream>
#include<stdexcept>
#include<utility>
#include<memory>
#include<algorithm>

using namespace std;

template<typename T>
class my_arr
{
    private:
        unique_ptr<T[]> data;
        size_t size;

    public:
        // Constructor
        my_arr(size_t n) : data(make_unique<T[]>(n)), size(n){
            cout << "Constructor called \n";
        }
        
        // Destructor
        ~my_arr(){
            cout << "Destructor called \n";
        }

        // Copy Constructor
        // generic 으로 바뀌어도, type이 T로 바뀐 것 빼고는 똑같다.
        my_arr(const my_arr& other) :
            // std::make_unique<>를 써야 한다는 것을 잊지 마라!!
            data(make_unique<T[]>(other.size)),
            size(other.size){
                // unique_ptr을 사용하기는 하지만, 해당 포인터가 가리키는 것은
                // 여전히 원시 배열이기 때문에, unique_ptr 클래스 내부의
                // .get()함수를 써서 하는수 없이 포인터 연산을 해야 한다.
                std::copy(other.data.get(), other.data.get()+size, data.get());
                cout << "Copy Constructor called \n";
        }

        // Copy Assignment Operator
        my_arr& operator=(const my_arr& other){
            if(this != &other){
                data = make_unique<T[]>(other.size);
                size = other.size; 
            }
            std::copy(other.data.get(), other.data.get()+size, data.get());
            cout << "Copy Assignment called \n";
            return *this;
        }

        // Move Constructorj : 여기에 전달된 인자는 const여서는 안 된다!!
        my_arr(my_arr&& other) noexcept : 
            // 이동 생성자에서 사용된 std::move()는 실제 이동을 하지 않고,
            // 단지 인자로서 받은 source object의 소유권이 바뀌게 해준다.
            // 이로써 deep copy를 하지 않게 해준다.
            data(std::move(other.data)),
            size(other.size){
                other.size = 0;
                cout << "Move Constructor called \n";
            }

        // Move Assignment Operator : 여기에 전달된 인자에도 const가 아니어아 한다!!
        my_arr& operator=(my_arr&& other){
           if(this == &other) return *this;

            data = std::move(other.data);
            size = other.size;
            
            other.size = 0;
            cout << "Move Assignment Operator called \n";
            return *this;
        }

        // define [] operator
        T& operator[](int idx){
            if(idx < 0 || idx >= size){
                throw std::out_of_range("Array index out of range!");
            }
            return data[idx];
        }

        // iterators : 이것들이 있어야 range for 루프를 쓸 수 있다.
        T* begin() {return data.get();}
        //const T* begin() {return data.get();}
        T* end() {return data.get()  + size;}
        //const T* end() {return data.get() + size;}

};//end of class

class node {
    public:
        int key;
        std::string val;
        
        // 적절한 생성자가 정의돼 있어야 myNode{0, "agc"s}; 와 같은 초기화가 가능하다.
        node(int k, std::string v) : key{k}, val{v}{}
        node() : key{0}, val{""} {}

        // operator << 를 오버로딩 해야 cout << node{1,"abc"s} << "\n";
        // 같은 코드가 의도대로 작동한다.
        // 또한 friend로 정의하는 것이 정석적인 방법이다.
        friend std::ostream& operator<<(std::ostream& os, const node& n){
            os << "{ key: " << n.key << ", val: " << n.val << " }";
            return os;
        }
};

my_arr<std::string> get_string_arr(size_t n){
    my_arr<std::string> instance(n);
    return instance;
}

my_arr<int> get_int_arr(size_t n){
    my_arr<int> instance(n);
    return instance;
}

my_arr<node> get_node_arr(size_t n){
    my_arr<node> instance(n);
    return instance;
}

int main(){

    // make and use of std::string arr
    my_arr<std::string> strArr = get_string_arr(2);
    // 접미사 s를 붙이면 스트링 리터럴이 아니라 std::string 타입의 값이 된다.
    strArr[0] = "Dongvin"s;
    strArr[1] = "Park"s;
    cout << "\n\t print std::string arr elems\n";
    for(auto e : strArr){
       cout << e << "\n"; 
    }
    
    // make and use of int arr
    my_arr<int> intArr = get_int_arr(2);
    intArr[0] = 0;
    intArr[1] = 1;
    
    cout << "\n\t print std::string arr elems\n";
    for(auto e : intArr){
       cout << e << "\n"; 
    }
    
    // make and use of custom node class arr
    my_arr<node> nodeArr = get_node_arr(3);
    // node 클래스 안에 적절한 생성자가 없을 경우, 아래의 '=' 할당 연산이 작동하지 않는다.
    nodeArr[0] = {0, "Dongvin"s};
    nodeArr[1] = {1, "Park"s};
    
    cout << "\n\t print std::string arr elems\n";
    for(auto e : nodeArr){
       cout << e << "\n"; 
    }

    return 0;
}
