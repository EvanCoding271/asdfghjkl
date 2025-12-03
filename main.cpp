#include <iostream>
#include <string>
#include <limits>
using namespace std;

    int main() {
    string name;
    cout << "Enter your name: ";
    getline(cin, name);

    cout << "Hello, " << name << "!" << endl;

    cout << "Press Enter to exit...";
    cin.get();

    
    return 0;
    }
