#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
using namespace std;

bool isPrime(int n) {
	if (n<2) return false;
	for(int i = 2; i * i <= n; i++) 
		if(n % i == 0) return false;
	return true;
}

int main() {
	const int n = 10;
	int pipes[n][2];
	for(int i = 0; i < n; i++)
		pipe(pipes[i]);

	for(int i = 0; i < n; i++) {
		pid_t p = fork();

		if(p == 0) {
			close(pipes[i][0]); // procesul copil scrie

			int start = i * 1000 + 1;
			int end = (i + 1) * 1000;

			string primes = "";
			for(int n = start; n <= end; n++) {
				if(isPrime(n))
					primes += to_string(n) + " ";
			}

			write(pipes[i][1], primes.c_str(), primes.size());
			close(pipes[i][1]);
			return 0;
		}
		close(pipes[i][1]); // procesul parinte citeste
	}

	// Procesul parinte citeste rezultatele
	for(int i = 0; i < n; i++) {
		char buffer[5000];
		int r = read(pipes[i][0], buffer, sizeof(buffer) - 1);
		buffer[r] = '\0';
		cout << "Process " << i << ": " << buffer << endl;
		close(pipes[i][0]);
	}

	for(int i = 0; i < n; i++) wait(NULL);

	return 0;
} 
