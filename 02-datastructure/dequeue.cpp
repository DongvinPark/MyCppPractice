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

	void offerFirst(T input) {}

	T* pollFirst() {}

	T* peekLast() {
		if (isEmpty()) {
			cout << "Dequeue is empty!\n";
			return nullptr;
		}
		return data[last];
	}

	void offerLast(T input) {}

	T* pollLast() {}

	void printDequeue() {
		if (isEmpty()) {
			cout << "Dequeue is empty!\n";
			return;
		}
		int head = first;
		int cnt = 0;
		while (true) {
			cout << data[head];
			head++;
			if (head >= capa) head = 0;
			cnt++;
			if (cnt == size) break;
		}
		cout << "\n";
	}

};

int main() {
	FixedDeque<int> first{3};
	return 0;
}
