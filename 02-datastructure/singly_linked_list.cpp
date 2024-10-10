#include<iostream>
#include<stdexcept>

class int_node {
    private:
        int elem;
        int_node* next;
    public:
        int_node() : elem{0}, next{nullptr} {}
        int_node(int val, int_node* next_node) : elem{val}, next{next_node} {}
        int getElem(){return elem;}
        int_node* getNext(){return next;}
        void setNext(int_node* next_node){next = next_node;}
};

class SLL {
    private:
        int_node* head;
        int_node* tail;
        int sz;
    public:
        // Default Constructor
        SLL() : head{nullptr}, tail{nullptr}, sz{0} {}

        // Destructor : 만들어야 되는가?
        
        // int container Constructor? ex: vector, array, forward_list, list..
        
        // Copy Constructor

        // Copy Assignment Operator

        // Move Constructor : 컴파일러의 RVO에 의해서 거의 호출되지 않으므로 생략.

        // Move Assignment Operator
        
        int size(){return sz;}
        
        bool isEmpty(){return sz == 0;}
        
        int getFirst(){
            if(isEmpty()) return -1; 
            // return head.getElem();는 안 된다. head 는 포인터이기 때문에,
            // 내부 함수에 접근하기 위해서는 역참조 연산(*)을 먼저 해야 한다.
            // return (*head).getElem();이 정답이고, return head->getElem();으로
            // 대체 가능하다.
            return head -> getElem();
        }
       
        int getLast(){
            if(isEmpty()) return -1;
            return tail -> getElem();
        }

        void addFirst(){

        }

        void addLast(){

        }

        void addElemBefore(){

        }

        void removeTargetElem(){

        }

        int removeFirst(){
            return -1;
        }

        int removeLast(){
            return -1;
        }

        void printList(){

        }

};

int main(){

    return 0;
}
