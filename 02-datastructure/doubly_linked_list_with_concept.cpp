#include <iostream>

using namespace std;

template<typename T>
concept PrinnableAndComparable = requires(T a, T b, std::ostream& os)
{
  {os << a} -> std::same_as<std::ostream&>;
  {a==b} -> std::same_as<bool>;
};

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

template<PrinnableAndComparable T>
class DLL {
  private:
  Node<T>* header;
  Node<T>* trailer;
  size_t sz;

  public:
  // Constructor
  explicit DLL() : header{nullptr}, tailer{nullptr}, sz{0} {}

  // Destructor

  // Copy Constructor

  // Copy Assignment operator

  // Move Constructor : 컴파일러의 RVO 동작에 의해서 거의 시행되지 않으므로 생략

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

  void addFirst(T e) {}

  void addLast(T e) {}

  void removeFirst() {}

  void removeLast() {}

  void addFirst() {}

  void addLast() {}

  void addElemBefore() {}

  bool removeTarget() {
    return false;
  }

  bool addBetween(T e) {
    return true;
  }

  void printList() {

  }

  void printListReverse() {

  }

  // Iterator
  class Iterator {

  };
};

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

};

int main() {


  return 0;
}// end of main
