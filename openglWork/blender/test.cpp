#include<iostream>
using namespace std;

void functionA(int some[]){
	some[0] = 1;
	some[1] = 2;
}

int main(){
	int some[2];
	functionA(some);
	cout << some[0] << endl;
	cout << some[1] << endl;
	return 0;
}
