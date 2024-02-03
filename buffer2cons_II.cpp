#include <iostream>
#include <vector>
#include <string>
#include <pthread.h>
#include "monitor.h"

int const threadsCounts = 5;  // number of threads
int const bufferSize = 9;

class Buffer
{
private:
    std::vector<int> values;
    Semaphore mutex;
    Semaphore full;
    Semaphore empty;
    Semaphore semA;
    Semaphore semB;
    Semaphore semC;

    bool readA, readB, readC;

    void print(std::string name)
    {
        std::cout << "\n### " << name << " " << values.size() << "[";
        for (auto v : values)
            std::cout << v << ",";
        std::cout << "]###\n";
    }

public:
    Buffer() : mutex(1), readA(false), readB(false), readC(false), full(bufferSize), empty(0), semA(1), semB(1), semC(1)
    {
    }

    void put(int value)
    {
        full.p();
        mutex.p();
        values.push_back(value);
        print("P");
        if (values.size() > 3)
            empty.v();
        mutex.v();
    }

    int getA()
    {
        semA.p();
        empty.p();
        mutex.p();
        int v = values.front();
        readA = true;
        print("A read");
        if (readB && readC)
        {
            values.erase(values.begin());
            print("A remove");
            readA = readB = readC = false;
            semA.v();
            semB.v();
            semC.v();
            full.v();
        }
        else
            empty.v();

        mutex.v();
        return v;
    }

    int getB()
    {
        semB.p();
        empty.p();
        mutex.p();
        int v = values.front();
        readB = true;
        print("B read");
        if (readA && readC)
        {
            values.erase(values.begin());
            print("B remove");
            readA = readB = readC = false;
            semA.v();
            semB.v();
            semC.v();
            full.v();
        }
        else
            empty.v();
        mutex.v();
        return v;
    }

    int getC()
    {
        semC.p();
        empty.p();
        mutex.p();
        int v = values.front();
        readC = true;
        print("C read");
        if (readA && readB)
        {
            values.erase(values.begin());
            print("C remove");
            readA = readB = readC = false;
            semA.v();
            semB.v();
            semC.v();
            full.v();
        }
        else
            empty.v();
        mutex.v();
        return v;
    }
};

Buffer buffer;

void *threadProd(void *arg)
{
    for (int i = 0; i < 10; ++i)
    {
        buffer.put(i);
    }
    return NULL;
}

void *threadConsA(void *arg)
{
    for (int i = 0; i < 17; ++i)
    {
        auto value = buffer.getA();
    }
    return NULL;
}

void *threadConsB(void *arg)
{
    for (int i = 0; i < 17; ++i)
    {
        auto value = buffer.getB();
    }
    return NULL;
}

void *threadConsC(void *arg)
{
    for (int i = 0; i < 17; ++i)
    {
        auto value = buffer.getC();
    }
    return NULL;
}

int main()
{
    pthread_t tid[threadsCounts];

    pthread_create(&(tid[0]), NULL, threadProd, NULL);
    pthread_create(&(tid[1]), NULL, threadProd, NULL);
    pthread_create(&(tid[2]), NULL, threadConsA, NULL);
    pthread_create(&(tid[3]), NULL, threadConsB, NULL);
    pthread_create(&(tid[4]), NULL, threadConsC, NULL);

    for (int i = 0; i < threadsCounts; i++)
        pthread_join(tid[i], (void **)NULL);

    return 0;
}
