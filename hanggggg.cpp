#include <iostream>
using namespace std;
int main(){
    char name[50],guess[50];
      cout << "enter name\n";
      getchar;
    cin >> name;

    cout << name;
    while(1){
      cout << "enter guess\n";
    cin >> guess;
    cout << guess;
    if(guess == name) {cout << "yopu won\n";
    return 0;}
    cout << "enter guess\n";
    cin >> guess;}
}