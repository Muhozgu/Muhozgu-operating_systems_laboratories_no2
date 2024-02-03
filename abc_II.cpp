#include <iostream>
#include <string>
#include <pthread.h>
#include "monitor.h"

int const threadsCounts = 3;  // number of threads

int const numberOfLettersA = 30;
int const numberOfLettersB = 10;
int const numberOfLettersC = 20;

std::string s;

Semaphore semA(0);
Semaphore semB(1);
Semaphore semC(0);

int countA = 0;
int countC = 0;


void writeB()
{
    semB.p();
    std::cout << "B" << std::flush;
    s += "B";
    semC.v();
}

void writeC()
{
    semC.p();
    std::cout << "C" << std::flush;
    s += "C";
    countC++;
    if (countC == 2)
    {
        semA.v();
        countC = 0;
    }
    else
    {
        semC.v();
    }
}

void writeA()
{
    semA.p();
    std::cout << "A" << std::flush;
    s += "A";
    countA++;
    if (countA == 3)
    {
        semB.v();
        countA = 0;
    }
    else
    {
        semA.v();
    }
}




void* threadA(void* arg)
{
    for (int i = 0; i < numberOfLettersA; ++i)
    {
        writeA();
    }
    return NULL;
}

void* threadB(void* arg)
{
    for (int i = 0; i < numberOfLettersB; ++i)
    {
        writeB();
    }
    return NULL;
}

void* threadC(void* arg)
{
    for (int i = 0; i < numberOfLettersC; ++i)
    {
        writeC();
    }
    return NULL;
}

int main()
{
    pthread_t tid[threadsCounts];

    // create the threads
    pthread_create(&(tid[0]), NULL, threadA, NULL);
    pthread_create(&(tid[1]), NULL, threadB, NULL);
    pthread_create(&(tid[2]), NULL, threadC, NULL);

    // wait for threads completion
    for (int i = 0; i < threadsCounts; i++)
        pthread_join(tid[i], (void**)NULL);

    std::cout << "\n";

    
    std::cout << "s=" << s << "\n";
    return 0;
}