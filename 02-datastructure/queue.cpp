#include<iostream>

using namespace std;

template<typename T>
class Queue{
    private:
        T* data;
        int ptr;
        int size;
        int capa;

    public:
        // Constructor
        explicit Queue(int cap) : data{new T[cap]}, size{0}, capa{cap},  ptr{0} {}

        // Destructor
        ~Queue(){
            delete[] data;
        }

        // Copy Constructor
        Queue(const Queue& other) : data{new T[other.capa]}, ptr{other.ptr}, size{other.size}, capa{other.capa} {
            std::copy(other.data, other.data + other.capa, data);
        }

        // Copy Assignment Operator
        Queue& operator=(const Queue& other) {
            if (this != &other) {
                delete[] data;
                data = new T[other.capa];
                ptr = other.ptr;
                size = other.size;
                capa = other.capa;
                std::copy(other.data, other.data + other.capa, data);
            }
            return *this;
        }

        // Move Constructor
        Queue(Queue&& other) noexcept : data{other.data}, ptr{other.ptr}, size{other.size}, capa{other.capa} {
            other.data = nullptr;
            other.size = 0;
        }

        // Move Assignment Operator
        Queue& operator=(Queue&& other) noexcept {
            if (this != &other) {
                delete[] data;
                data = other.data;
                ptr = other.ptr;
                size = other.size;
                capa = other.capa;
                other.data = nullptr;
                other.size = 0;
            }
            return *this;
        }

        int getSize(){return size;}
        bool isEmpty(){return size==0;}

        void inQueue(const T& elem){
            if(size >= capa){
                cout << "queue is full!\n";
                return;
            }
            int idx = (ptr + size)%capa;
            data[idx] = elem;
            size++;
        }

        // 리턴 타입이 '참초'일 때는 nullptr를 리턴할 수 없다.
        // nullptr도 어쨌든 포인터이기 때문이다.
        T* peekFirst(){
            if(isEmpty()){
                cout << "queue is empty!\n";
                return nullptr;
            }
            return data[ptr];
        }

        T* deQueue(){
            if(isEmpty()){
                cout << "queue is empty!\n";
                return nullptr;
            }
            T* result = &data[ptr];

            // 일단 ptr를 무조건 전진 시키는 것이 맞다.
            // 그 다음에 capa(== data배열의 마지막 인덱스보다 1 큰 값)
            // 에 도달했을 때만 ptr을 0으로 초기화 해서 순환하게 만든다.
            ptr++;
            if(ptr >= capa) {
                ptr = 0;
            }
            //ptr = (ptr+1)%capa; // 혹은 이 로직으로 대체해도 된다.

            size--;
            return result;
        }

        void printQueue(){
            if(isEmpty()){
                cout << "queue is empty!\n";
                return;
            }
            int head = ptr;
            int cnt = 0;
            while(true){
                cout << data[head] << ", ";
                head++;
                if(head >= capa){
                    head = 0;
                }
                //head = (head+1)%capa;
                cnt++;
                if(cnt >= size) break;
            }
            cout << "\n";
        }
};

int main(){

    Queue<int> first{3};

    cout << "print queue when made\n";
    first.printQueue();

    first.inQueue(1);
    first.inQueue(2);
    first.inQueue(3);
    first.deQueue();
    first.inQueue(4);

    first.printQueue();// expect : 2,3,4,

    cout << "deQueue until queue is empty!\n";
    while(first.isEmpty()==false){
        int* value = first.deQueue();
        cout << "dequeued elem : "  << *value << "\n";
    }
    cout << "\n";

    first.printQueue();

    return 0;
}
