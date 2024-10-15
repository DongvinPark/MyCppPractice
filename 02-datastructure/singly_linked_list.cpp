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
                head = head -> getNext(); // 다음 노드를 먼저 가리키게 만든다.
                                          
                                          // &temp를 출력하면 temp가 가리키는 노드의 주소가 아니라,
                                          // temp 라는 포인터 변수 자체의 메모리 주소가 출력된다.
                                          // temp라는 포인터 변수가 가리키는 노드 클래스의 주소를
                                          // 출력하고 싶다면 그냥 cout << temp 라고 해야 한다.
                                          // 생각해보면, temp 자체가 포인터 변수이기 때문에
                                          // 여기에 뭔 짓을 하지 않은 채로 출력을 해야 비로소
                                          // temp가 원래 가리키던 객체(int_node)의 주소를 출력할
                                          // 수 있는 것.
                cout << "Destructor released node !! : " << temp << " : val : " << temp->getElem() << "\n";
                delete temp; // 자원을 회수 한다.
            }
        }
        
        // int container Constructor? ex: vector, array, forward_list, list..
        
        // Copy Constructor
        // 복사 및 이동 생성자를 정의하거나, 복사 및 이동 연산자를 오버로딩 할 때는 
        // 정해진 인자 형식을 지켜야 한다. 그렇지 않으면 정의 되지 않은 걸로 간주되어, 호출되지 않는다.
        // 그리고 복사 작업을 시작하기 전에 현재 객체를 '비어 있는' 상태로 먼저 만들어야 한다!!
        SLL_int(const SLL_int& other) : head{nullptr}, tail{nullptr}, sz{0} {
            cout << "Copy Constructor called! \n";          
            if(other.sz==0) {
               return;
           } else {
                int_node* cur = other.head;
                while(true){
                    if(cur != nullptr){
                        int val = cur->getElem();
                        addLast(val);
                        cur = cur->getNext();
                    } else break;
                }
           }
        }

        // Copy Assignment Operator
        SLL_int& operator=(const SLL_int& other){
            if(this != &other && other.sz > 0){
                sz = 0;
                head = nullptr;
                tail = nullptr;
                int_node* cur = other.head;
                while(true){
                    if(cur != nullptr){
                        int val = cur->getElem();
                        addLast(val);
                        cur = cur->getNext();
                    } else break;
                }
            }
            cout << "Copy Assignment operator called!\n";
            return *this;
        }

        // Move Constructor : 컴파일러의 RVO에 의해서 거의 호출되지 않으므로 생략.

        // Move Assignment Operator
        SLL_int& operator=(SLL_int&& other) noexcept {
            if(this != &other && other.sz > 0){
                sz=0;
                head=nullptr;
                tail=nullptr;
                int_node* cur = other.head;
                while(true){
                    if(cur != nullptr){
                        int val = cur->getElem();
                        addLast(val);
                        cur = cur->getNext();
                    } else break;
                }
                other.clear();
            }
            cout << "Move Assignment operator called!\n";
            return *this;
        }
        
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

        void addElemAfter(int loc_val, int val){
            int_node* target_node = head;
            while(true){
                if(target_node->getElem()==loc_val) break;
                target_node = target_node->getNext();
            }
            if(target_node == nullptr)
                throw std::logic_error("No such element in list! : "s + to_string(loc_val));
            int_node* new_node = new int_node(val);
            int_node* next_for_new = target_node->getNext();
            target_node->setNext(new_node);
            new_node->setNext(next_for_new);
            sz++;
        }

        void removeFirstTargetElem(int val){
            int_node* before_target = nullptr;
            int_node* target_node = head;
            while(true){
                if(target_node->getElem()==val) break;
                before_target = target_node;
                target_node = target_node->getNext();
            }
            if(target_node == nullptr)
                throw std::logic_error("No such element in list! : "s + to_string(val));
            if(target_node == head) {
                removeFirst(); // sz-- 이미 포함돼 있다.
                return;
            }
            before_target -> setNext(target_node->getNext());
            delete target_node;
            sz--;
        }

        int removeFirst(){
            // java의 IllegalStateExction과 유사하다.
            if(isEmpty()) throw std::logic_error("List is empty!");
            int result = head->getElem();
            if(sz==1){
                delete head;
                head = nullptr;
                tail = nullptr;
            } else {
                int_node* next_head = head->getNext();
                delete head;
                head = next_head;
            }
            sz--;
            return result;
        }

        int removeLast(){
            if(isEmpty()) throw std::logic_error("List is empty!");
            int result = tail->getElem();
            if(sz==1){
                delete tail;
                head = nullptr;
                tail = nullptr;
            } else {
                // tail 바로 직전 노드를 찾아야 한다.
                int_node* new_tail = head;
                while(true){
                    if(new_tail -> getNext() -> getNext() == nullptr) break;
                    new_tail = new_tail -> getNext();
                }//wh
                delete tail;
                tail = new_tail;
                // 새로운 tail을 설정해준 후, 그 tail의 다음 노드가 nullptr가 되게 해줘야 한다!!
                tail -> setNext(nullptr);
            }
            sz--;// sz를 감소시키는 것도 까먹지 말자.
            return result;
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

        void clear(){
            if(isEmpty()){
                cout << "List is empty! \n";
            }
            int_node* cur = head;
            while(true){
                if(cur != nullptr){
                    int_node* next_target = cur->getNext();
                    cout << "cleared node addr : " << cur << "\n";
                    delete cur;
                    cur = next_target;
                } else break;
            }
            // head tail sz를 적절하게 초기화 하는 것을 까먹지 말자.
            head = nullptr;
            tail = nullptr;
            sz = 0;
        }
};

SLL_int createTempSLL_int(){
    SLL_int temp{};
    temp.addFirst(9);
    temp.addLast(10);
    return temp;
}

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
    first.removeFirst();
    first.removeLast();
    // expect : 312
    first.printList();

    SLL_int second{};
    second.printList();

    second.addFirst(1);
    second.addLast(2);
    second.addFirst(3);
    second.addLast(4);
    second.addFirst(5);
    cout << second.size() << "\n";
    // expect : 53124 
    second.printList();
    second.removeFirst();
    cout << "Size after removeFirst() : " << second.size() << "\n";
    second.removeLast();
    // expect : 312
    second.printList();
    cout << second.size() << "\n";
    second.removeFirst();
    cout << second.size() << "\n";
    second.printList();
    second.removeLast();
    second.removeLast();
    // second 리스트는 비어 있다.

    SLL_int third{};
    third.printList();
    third.addFirst(2);
    //third.addLast();
    third.printList();
    // expect : 2
    third.addElemAfter(2, 3);
    third.printList();
    //third.removeFirstTargetElem(2);
    //third.removeFirstTargetElem(3);
    //third.removeFirst();// throws logic_error.
    third.printList();

    //third.clear(); // released two nodes.
    
    SLL_int fourth = third;
    cout << "Print two lists after copy constructor call\n";
    third.printList();
    fourth.printList();

    SLL_int fifth{};
    fifth.addFirst(0);
    fifth.addLast(1);
    fifth.addLast(2);
    
    SLL_int sixth{};
    sixth = fifth;
    cout << "Print two lists after copy assignment operator call\n";
    fifth.printList();
    sixth.printList();
    fifth.clear();
    sixth.clear();

    SLL_int seventh{};
    seventh = createTempSLL_int();
    cout << "Print list ater move assignment operator call\n";
    seventh.printList();
    seventh.clear();

    return 0;
}
