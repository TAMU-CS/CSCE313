#include <iostream> 
#include <string>
using namespace std;

int main(){
    cout << "inside child..." << endl;
    string temp;
    while(getline(cin, temp)){
        cout << temp << endl;
    }

    cout << "inside child over..." << endl;

    return 0;
}