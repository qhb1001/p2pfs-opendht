#include <thread>
#include <iostream>
#include <string>
using namespace std;

typedef void (*printString) (string);

void func(string x) {
	cout << "hello world from " << x << endl;
}

void foo(printString funcPtr, int x) {
	funcPtr("hongbo");
	cout << "I got the value " << x << endl;
}

int main() {
	thread th(foo, func, 521);
	th.join();
}

