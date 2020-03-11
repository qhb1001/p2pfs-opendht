#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <pthread.h>

using namespace std;

int count = 1;
void hello(int x) {
	cout << "Hello world from " << x << endl;
}

void* routine(void *arg) {
	void (*funcPtr)(int){reinterpret_cast<void (*)(int)>(arg)};
	funcPtr(count++);
	pthread_exit(NULL);
}

int main() {
	pthread_t tid[2];
	for (int i = 0; i < 2; i++) {
		pthread_create(&tid[i], NULL, routine, (void*)hello);
	}

	for (int i = 0; i < 2; i++) {
		pthread_join(tid[i], NULL);
	}
	pthread_exit(NULL);
}
