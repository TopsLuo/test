#include <iostream>
#include <sys/time.h>
#include <unistd.h>
using namespace std;

int main()
{
    clock_t start, end;
    start = clock();
    usleep(5);
    end = clock();
    cout << end - start <<endl;
    cout << end <<endl;
    return 1;
}
