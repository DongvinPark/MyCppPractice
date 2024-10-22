#include <iostream>

using namespace std;
/*
template<typename T>
concept PrinnableAndComparable = requires(T a, T b, std::ostream& os)
{
  {os << a} -> std::same_as<std::ostream&>;
  {a==b} -> std::same_as<bool>;
};
*/
template<typename T>
class Node {
  private:
  T elem;
  Node* prev;
  Node* next;

  public:
  explicit Node() : elem{}, prev{nullptr}, next{nullptr} {}
  explicit Node(T e) : elem{e}, prev{nullptr}, next{nullptr} {}
  explicit Node(T e, Node* p, Node* n) : elem{e}, prev{p}, next{n} {}

  T& getElem() {return elem;}
  Node* getNext() {return next;}
  Node* getPrev() {return prev;}
  void setNext(Node* n) {next = n;}
  void setPrev(Node* p) {prev = p;}
};

template</*PrinnableAndComparable*/typename T>
class DLL {
  private:
  Node<T>* header;
  Node<T>* trailer;
  size_t sz;

  bool addBetween(T e, Node<T>* p, Node<T>* n) {
      Node<T>* newNode = new Node<T>(e, p, n);
      p->setNext(newNode);
      n->setPrev(newNode);
      sz++;
      return true;
  }

  bool remove(Node<T>* target){
      Node<T>* pre = target->getPrev();
      Node<T>* nex = target->getNext();
      pre->setNext(nex);
      nex->setPrev(pre);
      sz--;
      delete target;
      return true;
  }

  public:
  // Constructor
  explicit DLL() : header{nullptr}, trailer{nullptr}, sz{0} {
      header = new Node<T>();
      trailer = new Node<T>();
      trailer->setPrev(header);
      header->setNext(trailer);
  }

  // Destructor

  // Copy Constructor

  // Copy Assignment operator

  // Move Constructor : 컴파일러의 RVO 동작에 의해서 거의 실행되지 않으므로 생략

  // Move Assignment operator

  size_t size() {
    return sz;
  }

  bool isEmpty() {
    return sz == 0;
  }

  T& getFirst() {
    return header->getNext()->getElem();
  }

  T& getLast() {
    return trailer->getPrev()->getElem();
  }

  bool addFirst(T e) {
      return addBetween(e, header, header->getNext());
  }

  bool addLast(T e) {
      return addBetween(e, trailer->getPrev(), trailer);
  }

  bool removeFirst() {
      if(isEmpty()){
        cout << "List is Empty!\n";
        return false;
      }
      return remove(header->getNext());
  }

  bool removeLast() {
      if(isEmpty()){
        cout << "List is Empty!\n";
        return false;
      }
      return remove(trailer->getPrev());
  }

  bool addElemBefore(T val, T newVal) {
      if(isEmpty()){
          cout << "List is Empty!\n";
          return false;
      }
      Node<T>* target = header->getNext();
      while(true){
          if(target == trailer){
              cout << "Cannot find target!\n";
              return false;
          }
          if(target->getElem() == val) break;
          target = target->getNext();
      }
      if(target == header->getNext()){
          return addFirst(newVal);
      } else {
          return addBetween(newVal, target->getPrev(), target);
      }
  }

  bool removeTarget(T val) {
      if(isEmpty()){
          cout << "List is Empty!\n";
          return false;
      }
      Node<T>* target = header->getNext();
      while(true){
          if(target == trailer){
              cout << "Cannot find target!\n";
              return false;
          }
          if(target->getElem() == val) break;
          target = target->getNext();
      }
      return remove(target);
  }

  int clear(){
      int cnt = 0;
      Node<T>* target = header->getNext();
      while(true){
          if(isEmpty()) break;
          Node<T>* nextTarget = target->getNext();
          remove(target);
          cnt++;
          target = nextTarget;
      }
      return cnt;
  }

  void printList() {
      if(isEmpty()){
        cout << "List is Empty!\n";
        return;
      }
      Node<T>* cur = header;
      while(true){
          cur = cur->getNext();
          if(cur->getNext() != nullptr){
              cout << cur->getElem() << ", ";
          } else break;
      }
      cout << "\n";
  }

  void printListReverse() {
    if(isEmpty()){
      cout << "List is Empty!\n";
      return;
    }
      Node<T>* cur = trailer;
      while(true){
          cur = cur->getPrev();
          if(cur->getPrev() != nullptr){
              cout << cur->getElem() << ", ";
          } else break;
      }
      cout << "\n";
  }

  // Iterator
  class Iterator {
      private:
      Node<T>* cur;

      public:
      Iterator(Node<T>* node) : cur{node} {}

      const T& operator*(){
          return cur->getElem();
      }

      Iterator& operator++(){
          if(cur != nullptr) cur = cur->getNext();
          return *this;
      }

      // 반복자를 이용한 역방향 순회도 가능하게 함.
      Iterator& operator--() {
          if(cur != nullptr) cur = cur->getPrev();
          return *this;
      }

      bool operator!=(const Iterator& other){
          return cur != other.cur;
      }

  };

  Iterator begin() {return Iterator(header->getNext());}
  Iterator end() {return Iterator(trailer);}

  // 역방향 순회를 위한 함수 정의
  Iterator rbegin() {return Iterator(trailer->getPrev());}
  Iterator rend() {return Iterator(header);}


};// end of DLL

class MyData {
  public:
  int key;
  std::string val;

  friend std::ostream& operator<<(std::ostream& os, const MyData& obj){
    os << "Key: " << obj.key << ", Value: " << obj.val;
    return os;
  }

  friend bool operator==(const MyData& before, const MyData& after){
    return before.key == after.key && before.val == after.val;
  }

};

class InvalidData {
  public:
  int key;
  std::string val;

  /*friend std::ostream& operator<<(std::ostream& os, const InvalidData& obj){
    os << "Key: " << obj.key << ", Value: " << obj.val;
    return os;
  }

  friend bool operator==(const InvalidData& before, const InvalidData& after){
    return before.key == after.key && before.val == after.val;
  }*/
  void addLast() {}

};

int main(){
    
    DLL<int> first{};

    first.addLast(1);
    first.addFirst(2);
    first.addLast(3);
    first.printList(); // expect : 2, 1, 3,
    first.printListReverse(); // expect : 3, 1, 2,
    
    cout << "printList by Iterator!!\n";
    for(auto& elem : first){
        cout << elem << ", ";
    }
    cout << "\n";

    cout << "printList in reverse order using Itegator!!\n";
    for(DLL<int>::Iterator it = first.rbegin(); it != first.rend(); --it){
        cout << *it << ", ";
    }
    cout << "\n";

    first.addElemBefore(9, 0);
    first.addElemBefore(2, 0);
    first.addElemBefore(3, 7);
    first.addElemBefore(1,9);
    first.printList(); // expect : 0, 2, 9, 1, 7, 3,
    first.removeTarget(90);
    first.removeTarget(9);
    first.printList(); // expect : 0, 2, 1, 7, 3,

    first.removeLast();
    first.removeFirst();
    first.printList(); // expect : 2, 1, 7,
    cout << "Clear DLL! number of released node : " << first.clear() << "\n";

    return 0;
}// end of main