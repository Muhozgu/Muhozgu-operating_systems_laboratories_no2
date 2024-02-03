#include <iostream>
#include <vector>
#include <string>
#include "monitor.h"

int const threadsCounts = 5;  //number of threads

int const bufferSize = 7;


class Buffer
{
private:
	Semaphore mutex;
        Semaphore empty;
        Semaphore full;
        Semaphore turnA;
        Semaphore turnB;

	std::vector<int> values;

	void print(std::string name)
	{
		std::cout << "\n ###" << name << " " << values.size() << "[";
		for (auto v : values)
			std::cout << v << ",";
		std::cout << "] ###\n";
	}

public:
	Buffer() : mutex(1), empty(0), full(7), turnA(1), turnB(0)
	{
	}

	void put(int value)
	{
		full.p();
                mutex.p();
		values.push_back(value);
		print("P");
                mutex.v();
                empty.v();
	}

	int getA()
	{
                turnA.p();
		empty.p();
		mutex.p();
                auto value = values.front();
		values.erase(values.begin());
		print("A");
                mutex.v();
                full.v();
                turnB.v();
		return value;  
	}

	int getB()
	{
                turnB.p();
	        empty.p();
                mutex.p();
		auto value = values.front();
		values.erase(values.begin());
		print("B");
                mutex.v();
                full.v();
                turnA.v();
		return value;
	}
};

Buffer buffer;

void* threadProd(void* arg)
{
	for (int i = 0; i < 20; ++i)
	{
		buffer.put(i);
	}
	return NULL;
}

void* threadConsA(void* arg)
{
	for (int i = 0; i < 30; ++i)
	{
		auto value = buffer.getA();
	}
	return NULL;
}

void* threadConsB(void* arg)
{
	for (int i = 0; i < 30; ++i)
	{
		auto value = buffer.getB();
	}
	return NULL;
}

int main()
{
	pthread_t tid[threadsCounts];

	// creation of threads
	pthread_create(&(tid[0]), NULL, threadProd, NULL);
	pthread_create(&(tid[1]), NULL, threadProd, NULL);
	pthread_create(&(tid[2]), NULL, threadProd, NULL);
	pthread_create(&(tid[3]), NULL, threadConsA, NULL);
	pthread_create(&(tid[4]), NULL, threadConsB, NULL);

	// waiting for all threads to finish
	for (int i = 0; i < threadsCounts; i++)
		pthread_join(tid[i], (void**)NULL);

	return 0;
}