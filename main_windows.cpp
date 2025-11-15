#include <windows.h>
#include <iostream>
#include <string>
using namespace std;

bool isPrime(int n) {
    if (n < 2) return false;
    for (int i = 2; i * i <= n; i++)
        if (n % i == 0) return false;
    return true;
}

void runChild(int id) {
    int start = id * 1000 + 1;
    int end = (id + 1) * 1000;

    string primes = "";
    for (int n = start; n <= end; n++)
        if (isPrime(n))
            primes += to_string(n) + " ";

    cout << primes;
}

int main(int argc, char* argv[]) {

    // procesul copil
    if (argc == 3 && string(argv[1]) == "--child") {
        runChild(atoi(argv[2]));
        return 0;
    }

    // procesul parinte
    const int n = 10;
    HANDLE readPipe[n], writePipe[n];
    PROCESS_INFORMATION pi[n];

    for (int i = 0; i < n; i++) {
        SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
        CreatePipe(&readPipe[i], &writePipe[i], &sa, 0);
        SetHandleInformation(readPipe[i], HANDLE_FLAG_INHERIT, 0);

        STARTUPINFOA si = { sizeof(si) };
        si.hStdOutput = writePipe[i];
        si.hStdError  = writePipe[i];
        si.dwFlags = STARTF_USESTDHANDLES;

        char exePath[MAX_PATH];
        GetModuleFileNameA(NULL, exePath, MAX_PATH);

        string cmd = string("\"") + exePath + "\" --child " + to_string(i);

        CreateProcessA(
            NULL, (LPSTR)cmd.c_str(),
            NULL, NULL,
            TRUE, 0,
            NULL, NULL,
            &si, &pi[i]
        );

        CloseHandle(writePipe[i]); // procesul parinte nu scrie
    }

    // citire
    for (int i = 0; i < n; i++) {
        char buffer[5000];
        DWORD bytesRead;

        ReadFile(readPipe[i], buffer, sizeof(buffer)-1, &bytesRead, NULL);
        buffer[bytesRead] = '\0';

        cout << "Process " << i << ": " << buffer << endl;
        CloseHandle(readPipe[i]);
    }

    // inchide procesele copil
    for (int i = 0; i < n; i++) {
        WaitForSingleObject(pi[i].hProcess, INFINITE);
        CloseHandle(pi[i].hProcess);
        CloseHandle(pi[i].hThread);
    }
    return 0;
}
