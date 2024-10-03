#include<iostream>
#include<stdexcept>
#include<utility>

class MyArray {
    private:
        int* data;
        size_t size;

    public:
        //Constructor
        MyArray(size_t n) : size(n) {
            data = new int[size];
        }
}
