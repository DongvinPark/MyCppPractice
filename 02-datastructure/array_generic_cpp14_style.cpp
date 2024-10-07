#include<iostream>
#include<stdexcept>
#include<utility>
#include<memory>
#include<algorithm>

using namespace std;

template<typename T>
class my_arr
{
    private:
        unique_ptr<T> data;
        size_t size;

    public:
        my_arr(size_t n) : data(make_unique<T[]>(n)), size(n){
            cout << "Constructor called \n";
        }

};//end of class
