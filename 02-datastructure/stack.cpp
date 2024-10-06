#include<iostream>
#include<stdexcept> // out of range 예외를 처리하기 위함.

using namespace std;

class Stack {
    public:
        // Constructor
        explicit Stack(int s) : 
            elem{new int[s]},
            sz{s},
            top_idx{-1}
        {
            if(s<=0){
                throw out_of_range("cannot make stack with size less than zero!");
            }
           for(int i=0; i<sz; i++){
               elem[i] = 0;
           }
           cout << "Constructor called \n";
        }

        // Copy Constructor
        Stack(const Stack& other) :
            elem{new int[other.sz]},
            sz{other.sz},
            top_idx{other.top_idx}
        {
            for(int i=0; i<sz; i++){
                elem[i] = other.elem[i];
            }
            cout << "Copy Constructor called \n";   Stack stack(5); 
        }
        
        // Move Constructor
        Stack(Stack&& other) :
            elem{other.elem},
            sz{other.sz},
            top_idx{other.top_idx}
        {
            other.elem = nullptr;
            other.sz = 0;
            other.top_idx = -1;
            cout << "Move Constructor called \n";
        }

        // Copy Assignment Operator
        Stack& operator=(const Stack& other){
            if(this == &other) return *this;
            delete[] elem;
            elem = new int[other.sz];
            sz = other.sz;
            top_idx = other.top_idx;
            for(int i=0; i<sz; i++){
                elem[i] = other.elem[i];
            }
            cout << "Copy Assignment operator called \n";
            return *this;
        }

        // Move Assignment Operator
        Stack& operator=(Stack&& other){
            // 자기 자신에게 이동시킬 경우 그냥 자기 자신 반환.
            if(this == &other) return *this;
            // 우선 내꺼 데이터를 먼저 지운다.
            delete[] elem;
            
            // 그 후, = 연산자 우측에 있는 걸로 내꺼를 초기화 한다.
            elem = other.elem;
            sz = other.sz;
            top_idx = other.top_idx;

            // 그 다음, = 연산자 우측에 있는 Stack의 필드들을 release한다.
            other.elem = nullptr;
            other.sz = 0;
            other.top_idx = -1;
            cout << "Move Assignment operator called \n";
            return *this;
        }

        // Destructor
        ~Stack() {
            // 배열을 new로 할당했다면, dlelte가 아니라, dlelte[]로 해제해야 한다.
            delete[] elem;
            cout << "Destructor called \n";
        }

        int& peek(){
            if(top_idx < 0){
                throw out_of_range("stack is empty!");
            }
            return elem[top_idx];
        };
        int& pop(){
            if(top_idx < 0){
                throw out_of_range("stack is empty or stack overflow!");
            }
            top_idx--;
            return elem[top_idx];
        };
        void push(int val){
            if(top_idx >= sz-1){
                throw out_of_range("stack is full!");
            }
            top_idx++;
            elem[top_idx] = val;
        };

        // C++에서는 멤버 이름과 함수 이름이 겹쳐서는 안 된다!!
        int size() const {
            // 스택의 사이즈는 elem 배열의 길이가 아니다!!
            return top_idx + 1;
        }
    
    private:
        int* elem;
        int sz;
        int top_idx;
};

/*
C++에서 함수 내부 지역범위에서 생성된 객체를 바깥으로 옮기기 위해서는 다음의 3가지 
방법이 주로 동원된다.

첫 번째 방법은 원시 포인터를 리턴하는 것이다.
이것은 리턴 받은 객체를 나중에 반드시 delete 해줘야 한다는 점에서 메모리 누수에 취약하다.
Stack* helper(){
    Stack* stack_in_function = new Stack(4);
    return stack_in_function;
}
Stack* myStack = helper();
// Use myStack
delete myStack; // Don't forget to free the memory!

두 번째 방법은 std::unique_ptr<Stack> 같이 스마트 포인터를 리턴하는 것이다.
별도로 해제를 해줄 필요가 없다는 점에서 편리하다.
std::unique_ptr<Stack> helper(){
    return std::make_unique<Stack>(4);
}

세 번째 방법은 값(value)을 리턴하는 것이다.
Stack helper(){
    Stack stack_in_function(4);
    return stack_in_function;
}

모던 C++에서는 세 번째 방법이 더 선호된다.
컴파일러에 의해서 Return Value Optimization이 실행되면서 불필요한 copy 동작이 일어나지 않기 때문이다.

>>> 아래의 코드는 잘못되 코드다.

Stack& helper(){
    Stack stack_in_function = Stack(4);
    return *stack_in_function;
}

지역 변수의 포인터를 리턴하려고 하면, 해당 지역 범위를 벗어날 때 자동으로 메소리가 해제되기 때문이다.
이때의 동작은 예측할 수 없다.
따라서 포인터를 리턴하려면 아예 원시포인터를 리턴하거나, unique_ptr를 리턴하고,
또는 세 번째 방법과 같이 컴파일러의 최적화가 일너날 수 있도도록 그냥 이름만 써서 '값' 리턴이 되게
하는 것이 맞다.
*/
Stack helper(){
    Stack stack_in_function = Stack(4);
    return stack_in_function;
}

int main(){
    // 이런 자바 스타일의 초기화 코드는 Cpp에서는 사용하지 말자.
    // 게다가 컴파일러가 copy elision을 하지 않을 경우,
    // 생서자 호출 >> 복사 생성자 호출 이라는 2단계를 거치게 된다.
    // Stack stack1 = Stack(5);
    
    // 따라서, 객체를 처음 생성할 때는
    // Stack myStack(5); 와 같이 직접 생성자를 호출하는 코드를 쓰자.

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
