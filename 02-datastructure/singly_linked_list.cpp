#include<iostream>
#include<stdexcept>

using namespace std;

// 아래와 같이 생성자와 get()... 종류의 멤버함수로 이루어진 클래스의 포인터도 
// 별도의 소멸자를 정의해줄 펼요 없이, delete (int_node type pointer); 로 자원 반납을 할 수 있다.
class int_node {
    private:
        int elem;
        int_node* next;
    public:
        int_node() : elem{0}, next{nullptr} {}

        int_node(int i) : elem{i}, next{nullptr} {}

        int_node(int val, int_node* next_node) : elem{val}, next{next_node} {}

        int getElem(){return elem;}
        int_node* getNext(){return next;}
        void setNext(int_node* next_node){next = next_node;}
};

class SLL_int {
    private:
        int_node* head;
        int_node* tail;
        int sz;
    public:
        // Default Constructor
        SLL_int() : head{nullptr}, tail{nullptr}, sz{0} {}

        // Destructor
        ~SLL_int(){
            while(head != nullptr){
                int_node* temp = head;
                head = head -> getNext(); // 다음 노드를 먼저 가리키게 만든 다음,
                cout << "release node !! : " << &temp << " : val - " << temp->getElem() << "\n";
                delete temp; // 자원을 회수 한다.
            }
        }
        
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
        
        // C++에서 함수한테 뭔가를 전달할 때는, 
        // 값, 포인터, 참조 무조건 이 셋 중 하나다.
        // 특히, '값'으로 전달할 때는 함수 범위를 벗어나는 순간 '값'인자를 바탕으로
        // 함수 내부에 복사되서 생성된 데이터가 소멸하기 때문에, 잘못하면 dangling pointer를 만들 수 있다.
        void addFirst(int val){
          int_node* new_node = new int_node(val);
          if(isEmpty()){
              head = new_node;
              tail = new_node;
          }else {
              new_node -> setNext(head);
              head = new_node; 
          } 
          sz++;
        }

        void addLast(int val){
           int_node* new_node = new int_node(val);
           if(isEmpty()){
               head = new_node;
               tail = new_node;
           } else{
               tail -> setNext(new_node);
               tail = new_node;
           }
           sz++;
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
            if(isEmpty()) {
                cout << "No elements in this SLL_int object! \n";
                return;
            }
            int_node* p_head = head;
            while(true){
                if(p_head != nullptr){ 
                    cout << p_head -> getElem() << ", ";
                    p_head = p_head -> getNext();
                } else break;
            }
            cout << "\n";
        }
};

int main(){

    SLL_int first{};
    first.printList();

    first.addLast(1);
    first.addLast(2);
    first.addFirst(3);
    first.addLast(4);
    first.addFirst(5);
    // expect : 53124
    first.printList();

    SLL_int second{};
    second.printList();

    second.addFirst(1);
    second.addLast(2);
    second.addFirst(3);
    second.addLast(4);
    second.addFirst(5);
    // expect : 53124 
    second.printList();

    SLL_int third{};
    third.printList();
    third.addFirst(2);
    //third.addLast();
    third.printList();
    // expect : 2

    return 0;
}