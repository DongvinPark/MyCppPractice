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

	// Copy Assignment operator

	// Move Constructor

	// Move Assignment operator

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
