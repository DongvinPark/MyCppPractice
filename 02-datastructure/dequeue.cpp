#include<iostream>

using namespace std;

template<typename T>
class FixedDeque {
private:
	T* data;
	int first;
	int last;
	int size;
	int capa;

public:
	// Constructor
	explicit FixedDeque(int len) : 
		data{new T[len]}, first{0}, last{0}, size{0}, capa{len} {
		if (len <= 0) throw runtime_error("Cannot make Dequeue with zero or negative capacity");
	}

	// Destructor
	~FixedDeque() {delete[] data;}

	// Copy Constructor
	FixedDeque(const FixedDeque& other) :
		data{ new T[other.capa] },
		first{ other.first },
		last{ other.last },
		size{ other.size },
		capa{ other.capa } {
		for (int i = 0; i < capa; ++i) {
			data[i] = other.data[i];
		}
	}

	// Copy Assignment Operator
	FixedDeque& operator=(const FixedDeque& other) {
		if (this == &other) return *this;  // 자기자신에게 할당하는 것을 방지한다.

		// 원래 내꺼 데이터를 없앤다.
		delete[] data;

		// 내꺼를 싹 다 other 꺼로 초기화 한다.
		data = new T[other.capa];
		first = other.first;
		last = other.last;
		size = other.size;
		capa = other.capa;
		for (int i = 0; i < capa; ++i) {
			data[i] = other.data[i];
		}

		// 그러고나서, 나 자신에 대한 포인터를 반환한다.
		return *this;
	}

	// Move Constructor
	FixedDeque(FixedDeque&& other) noexcept :
		// 내꺼 데이터 포인터가 other 꺼 데이터를 가리키게 만든다.
		data{ other.data },
		first{ other.first },
		last{ other.last },
		size{ other.size },
		capa{ other.capa } {
		// other 꺼 데이터 포인터는 nullptr를 가리키게 만들고,
		// other의 다른 필드들도 전부 '비어 있는' 상태로 만든다.
		other.data = nullptr;
		other.size = 0;
		other.capa = 0;
	}

	// Move Assignment Operator
	FixedDeque& operator=(FixedDeque&& other) noexcept {
		if (this == &other) return *this;  // 자기 자신에 대한 할당 방지

		delete[] data;  // 내꺼를 먼저 싹 지워 놓는다.

		// 내꺼를 other 꺼로 초기화 한다.
		data = other.data;
		first = other.first;
		last = other.last;
		size = other.size;
		capa = other.capa;

		// other 꺼를 '비어 있는' 상태로 만든다.
		other.data = nullptr;
		other.size = 0;
		other.capa = 0;

		return *this;
	}

	int getSize() { return size; }
	bool isEmpty() { return size == 0; }

	T* peekFirst() {
		if (isEmpty()) {
			cout << "Dequeue is empty!\n";
			return nullptr;
		}
		return data[first];
	}

	void offerFirst(T input) {
		if (size == capa) {
			cout << "Dequeue is full!\n";
			return;
		}
		if (isEmpty()) { data[first] = input; }

		else if (first > 0) { first--; data[first] = input; }
		else if (first == 0) { first = (capa - 1); data[first] = input; }

		size++;
	}

	T* pollFirst() {
		if (isEmpty()) {
			cout << "Dequeue is empty!\n";
			return nullptr;
		}
		T* result = &data[first];

		if (first != capa - 1) { first++; }
		else if (first == capa - 1) { first = 0; }

		size--;

		return result;
	}

	T* peekLast() {
		if (isEmpty()) {
			cout << "Dequeue is empty!\n";
			return nullptr;
		}
		return data[last];
	}

	void offerLast(T input) {
		if (size == capa) {
			cout << "Dequeue is full!\n";
			return;
		}

		if (isEmpty()) { data[last] = input; }

		else if (last >= 0) { last++; data[last] = input; }
		else if (last == capa - 1) { last = 0; data[last] = input; }

		size++;
	}

	T* pollLast() {
		if (isEmpty()) {
			cout << "Dequeue is empty!\n";
			return nullptr;
		}
		T* result = &data[last];
		if (last != 0) { last--; }
		else if (last == 0) { last = capa - 1; }

		size--;
		return result;
	}

	void printDequeue() {
		if (isEmpty()) {
			cout << "Dequeue is empty!\n";
			return;
		}
		int head = first;
		cout << "head : " << head << "\n";
		int cnt = 0;
		while (true) {
			cout << data[head] << ", ";
			head++;
			if (head >= capa) head = 0;
			cnt++;
			if (cnt == size) break;
		}
		cout << "\n";
	}

};

int main() {
	try {
		// Create a FixedDeque with capacity of 5
		FixedDeque<int> deque(5);

		cout << "Adding elements to deque at the back:" << endl;
		deque.offerLast(1);
		deque.offerLast(2);
		deque.offerLast(3);
		deque.printDequeue();  // expect: 1 2 3

		cout << "\nAdding elements to deque at the front:" << endl;
		deque.offerFirst(0);
		deque.offerFirst(-1);
		deque.printDequeue();  // expect: -1 0 1 2 3

		cout << "\nPolling from the front:" << endl;
		int* frontElement = deque.pollFirst();
		if (frontElement) cout << "Polled from front: " << *frontElement << endl;
		deque.printDequeue();  // expect: 0 1 2 3

		cout << "\nPolling from the back:" << endl;
		int* backElement = deque.pollLast();
		if (backElement) cout << "Polled from back: " << *backElement << endl;
		deque.printDequeue();  // expect: 0 1 2

		cout << "\nFilling deque to capacity:" << endl;
		deque.offerLast(4);
		deque.offerLast(5);
		deque.printDequeue();  // expect: 0 1 2 4 5

		deque.offerLast(6);

	}
	catch (const runtime_error& e) {
		cout << "Exception: " << e.what() << endl;
	}

	return 0;
}
