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
        }

        // Copy Constructor?
        //
        // Move Constructor?
        //
        // Copy Assignment Operator?
        //
        // Move Assignment Operator?

        // Destructor
        ~Stack() {
            // 배열을 new로 할당했다면, dlelte가 아니라, dlelte[]로 해제해야 한다.
            delete[] elem;
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

int main(){

    Stack my_stack = Stack(1);

    my_stack.push(0);
    int top = my_stack.pop();

    cout << "top is : " << top << endl;

    my_stack.push(1);
    my_stack.push(1);

    return 0;
}
