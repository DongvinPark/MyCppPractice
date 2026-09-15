#include<iostream>
#include<stdexcept> // out of range 예외를 처리하기 위함.

using namespace std;

class Stack {
    public:
        // Constructor
        explicit Stack(int s)
            : elem{nullptr}, capacity{s}, top_idx{-1}
        {
            if (s <= 0)
            {
                throw invalid_argument("Stack size must be positive");
            }
            // 스택 사이즈 결정 인자 s에 대한 검사를 마친 후에 메모리 할당을 해야 비교적 안전하다.
            elem = new int[s];
        }
        /* 아래의 예전 코드는 위험한 생성자 코드다. int s 를 검사하기도 전에 elem 을 먼저 int s 로 초기화 해버리기 때문이다.
         explicit Stack(int s) :
            elem{new int[s]}, // s 가 -1 이라면? 여기가 곧바로 undefined behaviour 를 보일 수도 있다.
            sz{s},
            top_idx{-1}
        {
            if(s<=0){ throw out_of_range("stack size must be positive!"); }
           for(int i=0; i<sz; i++){ elem[i] = 0; }
           cout << "Constructor called \n";
        }*/

        // Copy Constructor
        Stack(const Stack& other) :
            elem{new int[other.capacity]},
            capacity{other.capacity},
            top_idx{other.top_idx}
        {
            for(int i=0; i<capacity; i++){
                elem[i] = other.elem[i];
            }
            cout << "Copy Constructor called \n";   Stack stack(5); 
        }
        
        // Move Constructor
        Stack(Stack&& other)  noexcept :
            elem{other.elem},
            capacity{other.capacity},
            top_idx{other.top_idx}
        {
            other.elem = nullptr;
            other.capacity = 0;
            other.top_idx = -1;
            cout << "Move Constructor called \n";
        }

        // Copy Assignment Operator
        Stack& operator=(const Stack& other)
        {
            if (this == &other) return *this;

            // 새 배열을 먼저 할당 및 초기화 한다. 이게 실패하더라도 원래 내꺼 배열은 무사하다.
            int* new_elem = new int[other.capacity];

            for (int i = 0; i<other.capacity; i++)
            {
                new_elem[i] = other.elem[i];
            }

            // 이제 내꺼 원본 배열 메모리를 해제한다.
            delete[] this->elem; // 이 코드는 다음과 완전히 똑같다. delete[] (*this).elem;

            elem = new_elem;
            capacity = other.capacity;
            top_idx = other.top_idx;

            return *this;
        }
        /*
         * 아래의 에전 구현은 틀린 것은 아니지만 new int[other.capacity]; 가 살패해서
         * std::bad_alloc 이 발생할 경우 문제가 될 수 있다.
         Stack& operator=(const Stack& other){
            if(this == &other) return *this;
            delete[] elem;
            elem = new int[other.capacity];
            capacity = other.capacity;
            top_idx = other.top_idx;
            for(int i=0; i<capacity; i++){
                elem[i] = other.elem[i];
            }
            cout << "Copy Assignment operator called \n";
            return *this;
        }*/

        // Move Assignment Operator
        Stack& operator=(Stack&& other) {
            // 자기 자신에게 이동시킬 경우 그냥 자기 자신 반환.
            if(this == &other) return *this;
            // 우선 내꺼 데이터를 먼저 지운다.
            delete[] elem;
            
            // 그 후, = 연산자 우측에 있는 걸로 내꺼를 초기화 한다.
            elem = other.elem;
            capacity = other.capacity;
            top_idx = other.top_idx;

            // 그 다음, = 연산자 우측에 있는 Stack의 필드들을 release한다.
            other.elem = nullptr;
            other.capacity = 0;
            other.top_idx = -1;
            cout << "Move Assignment operator called \n";
            return *this;
        }

        // Destructor
        ~Stack() {
            // 배열을 new ... [] 로 할당했다면, delete가 아니라, delete[]로 해제해야 한다.
            delete[] elem;
            cout << "Destructor called \n";
        }

        int peek(){
            if(top_idx < 0){
                throw out_of_range("stack is empty!");
            }
            return elem[top_idx];
        };
        int pop(){
            if(top_idx < 0){
                throw out_of_range("stack is empty!");
            }
            return elem[top_idx--];
        };
        void push(int val){
            if(top_idx >= capacity-1){
                throw out_of_range("stack is full!");
            }
            top_idx++;
            elem[top_idx] = val;
        };

        int size() const {
            // 배열의 capacity(sz)와 현재 stack에 들어있는 원소 수(size)는 다르다.
            return top_idx + 1;
        }
    
    private:
        int* elem;
        int capacity;
        int top_idx;
};

/*
C++에서 함수 내부 지역범위에서 생성된 객체를 바깥으로 옮기기 위해서는 다음의 3가지 
방법이 주로 동원된다.

// ❌ 잘못된 예시: 지역 객체의 주소를 반환하면 dangling pointer가 된다.
// helper_risky_dangling_ptr()가 반환되어 stack_in_function이 소멸한 뒤 그 포인터가 dangling pointer가 된다.
Stack* helper_risky_dangling_ptr(){
    Stack stack_in_function(4);
    return &stack_in_function;
}

// 올바른 예시들은 다음과 같다.
// ✅ 동적 할당(==원시 포인터를 리턴): 호출자가 소유권을 가져간다. 단, 호출자가 delete 해야 한다.
Stack* helper_raw_ptr() {
    return new Stack(4);
}

// ✅ 스마트 포인터: 소유권을 명확하게 전달한다.
std::unique_ptr<Stack> helper_smart_ptr() {
    return std::make_unique<Stack>(4);
}

// ✅ 값 반환: 현대 C++에서 가장 자연스러운 방법 중 하나
Stack helper_by_value() {
    return Stack(4);
}
*/

Stack helper(){ // 값 반환 방식.
    Stack stack_in_function(4);
    return stack_in_function;
}

int main(){
    // 객체를 처음 생성할 때는 Stack myStack(5); 와 같이 직접 생성자를 호출하는 코드가 가독성이 좋다.

    // Constructor 2 times.
    Stack stack3(1);
    Stack stack4(2);
    // Copy Assignment Operator
    stack3 = stack4;

    cout << "\n copy assignment operator checked \n\n";

    // Constructor
    Stack stack5(1);
    stack5.push(5);
    // Copy Constructor
    Stack stack6 = stack5;

    cout << "\n copy constructor call checked \n\n" << "top value of stack5 : " << stack5.peek() << "\n";

    Stack stack7 = helper();
    
    return 0; 
}
