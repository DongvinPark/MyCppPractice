#include<iostream>
using namespace std;

int main(){
    cout << "Enter anything (input will stop at single '|' and enter)\n";

    string input;
    while(true){
        string elem;
        getline(cin, elem);

        if(elem == "|"){
            break;
        }
        input += elem + " ";
    }

    cout << "your keyboard input is :\n" << input << "\n";

    return 0;
}
