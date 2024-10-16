#include<iostream>
#include<stdexcept>
#include<concepts>

using namespace std;

// singly_linked_list.cpp가 C++14에 최적화된 구현이었다면,
// 이번에는 C++20의 concept 기능을 활용해서 
// SLL 클래스를 인스턴스화 할 때, 노드의 elem 타입에 들어갈 클래스가
// 필수 연산자(==,<<)가 오버로딩 됐는지를 컴파일 타임에 체크하게 만들어 보았다.

//  g++ -std=c++20 -o executable singly_linked_list_with_concept.cpp && ./executable && rm executable
//  g++컴파일럿 C++ 20 버전으로 컴파일 하게 만드는 명령어다.
//  코드 수정 없이 바로 실행하면 concept의 기능에 의해서 프로그램이 컴파일 타임 에러 메시지를 생성한 후
//  종료 된다.
//  정상 실행시키고 싶다면, InvalidData 클래스 내부의 주석을 해제하거나, main 함수 내의
//  failedSLL 객체를 사용하는 코드를 전부 주석 처리하면 된다.
template<typename T>
concept PrintableAndComparable = requires(T a, T b, std::ostream& os){
    {os << a} -> std::same_as<std::ostream&>;
    {a==b} -> std::same_as<bool>;
};

template<typename T>
class Node {
    private:
        T elem;
        Node* next;
    public:
        Node() : elem{}, next{nullptr} {}

        Node(T val) : elem{val}, next{nullptr} {}

        Node(T val, Node* next_node) : elem{val}, next{next_node} {}

        T& getElem(){return elem;}

        Node* getNext(){return next;}
        void setNext(Node* next_node){next = next_node;}
};

template<PrintableAndComparable T>
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
                head = head -> getNext();
                cout << "Destructor released node !! : " << temp << " : val : " << temp->getElem() << "\n";
                delete temp;
            }
        }
         
        // Copy Constructor
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
            return head -> getElem();
        }
       
        T getLast(){
            if(isEmpty()) return -1;
            return tail -> getElem();
        }
        
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
                throw std::logic_error("No such element in list!");
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
                Node<T>* new_tail = head;
                while(true){
                    if(new_tail -> getNext() -> getNext() == nullptr) break;
                    new_tail = new_tail -> getNext();
                }//wh
                delete tail;
                tail = new_tail;
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

        friend bool operator==(MyData& before, MyData& after){
            return before.key == after.key && before.val == after.val;
        }
};

class InvalidData {
    private:
        std::string key;
        int val;
    public:
        InvalidData(std::string k, int v) : key{k}, val{v} {}
        /*
        friend std::ostream& operator<<(std::ostream& os, const InvalidData& obj){
            os << "Key: " << obj.key << ", Value: " << obj.val;
            return os;
        }

        friend bool operator==(InvalidData& before, InvalidData& after){
            return before.key == after.key && before.val == after.val;
        }
        */
};

int main(){
    
    cout << "\n" << "custom object SLL test after template T \n";
    
    SLL<MyData> customSLL{};
    MyData firstData{"a",1};
    MyData secondData{"b", 2};
    MyData thirdData{"c",3};
    customSLL.addFirst(firstData);
    customSLL.addLast(secondData);
    customSLL.addElemAfter(firstData, thirdData);
    customSLL.printList();

    // 아래의 코드를 주석 해제하면, concept의 기능이 실행되면서 
    // 컴파일 타임 에러가 난다. concept 에서 정의한 탸입 제한 내용인,
    // == 연산자 및 << 연산자 오버로딩이 되지 않은 클래스는 SLL 클래스의 인스턴스화가 허용되지
    // 않으면서 프로그램이 실행조차 되지 않는다.
    // 아래의 코드를 C++20 버전에서 정상 실행시키고 싶다면,
    // InvalidData 클래스 내부의 연산자 오버로딩 구현 부분을 주석 해제 하면 된다. 
    cout << "\n" << "type constraing fail test using concept in C++20 \n";
    SLL<InvalidData> failedSLL{};
    InvalidData fourthData{"d",4};
    InvalidData fifthData{"e", 5};
    InvalidData sixthData{"f",6};
    failedSLL.addFirst(fourthData);
    failedSLL.addLast(fifthData);
    failedSLL.addElemAfter(fourthData, sixthData);
    failedSLL.printList();
    

    return 0;
}
