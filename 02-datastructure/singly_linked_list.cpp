#include<iostream>
#include<stdexcept>

using namespace std;

// 아래와 같이 생성자와 get()... 종류의 멤버함수로 이루어진 클래스의 포인터도 
// 별도의 소멸자를 정의해줄 펼요 없이, delete (int_node type pointer); 로 자원 반납을 할 수 있다.
template<typename T>
class Node {
    private:
        T elem;
        Node* next;
    public:
        Node() : elem{}, next{nullptr} {}

        Node(T val) : elem{val}, next{nullptr} {}

        Node(T val, Node* next_node) : elem{val}, next{next_node} {}

        // iterator 클래스를 구현할 때, 이 함수에서 '참조'를 리턴하는 것이 매우 중요하다.
        // 여기서 참조가 아니라 T와 같이 그냥 값을 리턴할 경우 발생하는 문제는 
        // SSL 클래스의 Iterator 클래스 내부에서 설명하였다.
        T& getElem(){return elem;}

        Node* getNext(){return next;}
        void setNext(Node* next_node){next = next_node;}
};

template<typename T>
class SLL {
    private:
        Node<T>* head;
        Node<T>* tail;
        int sz;
    public:
        // Default Constructor
        SLL() : head{nullptr}, tail{nullptr}, sz{0} {}

        // Destructor
        ~SLL(){
            while(head != nullptr){
                Node<T>* temp = head;
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
        SLL(const SLL& other) : head{nullptr}, tail{nullptr}, sz{0} {
            cout << "Copy Constructor called! \n";          
            if(other.sz==0) {
               return;
           } else {
                Node<T>* cur = other.head;
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
        SLL& operator=(const SLL& other){
            if(this != &other && other.sz > 0){
                sz = 0;
                head = nullptr;
                tail = nullptr;
                Node<T>* cur = other.head;
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
        SLL& operator=(SLL&& other) noexcept {
            if(this != &other && other.sz > 0){
                sz=0;
                head=nullptr;
                tail=nullptr;
                Node<T>* cur = other.head;
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
        
        T getFirst(){
            if(isEmpty()) return -1; 
            // return head.getElem();는 안 된다. head 는 포인터이기 때문에,
            // 내부 함수에 접근하기 위해서는 역참조 연산(*)을 먼저 해야 한다.
            // return (*head).getElem();이 정답이고, return head->getElem();으로
            // 대체 가능하다.
            return head -> getElem();
        }
       
        T getLast(){
            if(isEmpty()) return -1;
            return tail -> getElem();
        }
        
        // C++에서 함수한테 뭔가를 전달할 때는, 
        // 값, 포인터, 참조 무조건 이 셋 중 하나다.
        // 특히, '값'으로 전달할 때는 함수 범위를 벗어나는 순간 '값'인자를 바탕으로
        // 함수 내부에 복사되서 생성된 데이터가 소멸하기 때문에, 잘못하면 dangling pointer를 만들 수 있다.
        void addFirst(T val){
          Node<T>* new_node = new Node<T>(val);
          if(isEmpty()){
              head = new_node;
              tail = new_node;
          }else {
              new_node -> setNext(head);
              head = new_node; 
          } 
          sz++;
        }

        void addLast(T val){
           Node<T>* new_node = new Node<T>(val);
           if(isEmpty()){
               head = new_node;
               tail = new_node;
           } else{
               tail -> setNext(new_node);
               tail = new_node;
           }
           sz++;
        }

        void addElemAfter(T loc_val, T val){
            Node<T>* target_node = head;
            while(true){
                if(target_node->getElem() == loc_val) break;
                target_node = target_node->getNext();
            }
            if(target_node == nullptr)
                throw std::logic_error("No such element in list! : "s + to_string(loc_val));
            Node<T>* new_node = new Node<T>(val);
            Node<T>* next_for_new = target_node->getNext();
            target_node->setNext(new_node);
            new_node->setNext(next_for_new);
            sz++;
        }

        void removeFirstTargetElem(int val){
            Node<T>* before_target = nullptr;
            Node<T>* target_node = head;
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

        T removeFirst(){
            // java의 IllegalStateExction과 유사하다.
            if(isEmpty()) throw std::logic_error("List is empty!");
            T result = head->getElem();
            if(sz==1){
                delete head;
                head = nullptr;
                tail = nullptr;
            } else {
                Node<T>* next_head = head->getNext();
                delete head;
                head = next_head;
            }
            sz--;
            return result;
        }

        T removeLast(){
            if(isEmpty()) throw std::logic_error("List is empty!");
            T result = tail->getElem();
            if(sz==1){
                delete tail;
                head = nullptr;
                tail = nullptr;
            } else {
                // tail 바로 직전 노드를 찾아야 한다.
                Node<T>* new_tail = head;
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
            Node<T>* p_head = head;
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
            Node<T>* cur = head;
            while(true){
                if(cur != nullptr){
                    Node<T>* next_target = cur->getNext();
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

        // iterators
        // 이게 있어야 for(audo elem : strList) {/*...*/} 같은 range for 활용이 가능하다.
        // range for를 사용해서 SSL 클래스의 노드들을 순회하기 위해서는 SSL 에서 
        // 반복자 클래스, 반복자를 리턴하는 begin() & end() 함수를 제공해줘야 하고,
        // 반복자 클래스에서는 세 종류의 연산자 즉, 역참조*, ++ 전진, 같지 않음!=
        // 에 대한 연산자 오벼로딩이 정의돼야 한다.
        class Iterator {
            private:
                Node<T>* cur;
            public:
                Iterator(Node<T>* node) : cur(node) {}
                
                // 특히, 역잠조 연산자를 오버로딩 할 때, 리턴 타입이 '참조'인데,
                // getElem() 함수는 T와 같이 값을 반환해 버리면 리턴 타입이 서로 맞지 않는 문제가 생긴다.
                const T& operator*() const {
                    return cur->getElem();
                }

                Iterator& operator++(){
                    if (cur != nullptr) cur = cur->getNext();
                    return *this;
                }

                bool operator!=(const Iterator& other){
                    return cur != other.cur;
                }
        };//end of Iterator        

        Iterator begin() {return Iterator(head);}
        Iterator end() {return Iterator(nullptr);}
};

SLL<int> createTempSLL_int(){
    SLL<int> temp{};
    temp.addFirst(9);
    temp.addLast(10);
    return temp;
}

class MyData {
    private:
        std::string key;
        int val;

    public:
        MyData(std::string k, int v) : key{k}, val{v} {}

        // cout << ... 를 위한 <<연산자를 오버로딩 할 때는 함수 시크니처에서 friend 키워드를 까먹지 말자..
        friend std::ostream& operator<<(std::ostream& os, const MyData& obj){
            os << "Key: " << obj.key << ", Value: " << obj.val;
            return os;
        }
};

int main(){
    
    SLL<int> first{};
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

    SLL<int> second{};
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

    SLL<int> third{};
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
    
    SLL<int> fourth = third;
    cout << "Print two lists after copy constructor call\n";
    third.printList();
    fourth.printList();

    SLL<int> fifth{};
    fifth.addFirst(0);
    fifth.addLast(1);
    fifth.addLast(2);
    
    SLL<int> sixth{};
    sixth = fifth;
    cout << "Print two lists after copy assignment operator call\n";
    fifth.printList();
    sixth.printList();
    fifth.clear();
    sixth.clear();

    SLL<int> seventh{};
    seventh = createTempSLL_int();
    cout << "Print list ater move assignment operator call\n";
    seventh.printList();
    seventh.clear();

    cout << "\n" << "string SLL test after template T \n";

    SLL<std::string> strList{};
    strList.addFirst("Dongvin");
    strList.addLast("Park");

    for(auto& elem : strList){
        cout << elem << " ";
    }
    cout << "\n";

    cout << "\n" << "custom object SLL test after template T \n";
    
    SLL<MyData> customSLL{};
    customSLL.addFirst(MyData{"a", 1});
    customSLL.addLast(MyData{"b", 2});
    customSLL.printList();

    return 0;
}
