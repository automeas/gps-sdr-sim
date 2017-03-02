#ifndef Array_Queue_H
#define Array_Queue_H
/* Queue - Circular Array implementation in C++*/
/* Lock free for single producer and single consumer*/
#include<iostream>
#include"gpssim.h"
using namespace std;
#define MAX_SIZE 10  //maximum size of the array that will store Queue. 
#define FRAME_SIZE 500000  //maximum size of the array that will store Queue. 
#include <boost/thread.hpp>


typedef struct
{
	gpstime_t time;
	short * iq_ptr;
} time_and_samples;

// Creating a class named ArrayQueue.
class ArrayQueue
{
private:
	time_and_samples A[MAX_SIZE];
	short buffer[MAX_SIZE][FRAME_SIZE];
	int front, rear;
	// To check wheter Queue is empty or not
	bool IsEmpty();

	// To check whether Queue is full or not
	bool IsFull();

	// Inserts an element in queue at rear end
	void Enqueue(gpstime_t time, short *sample, size_t length);

	// Removes an element in Queue from front end. 
	void Dequeue();
	/*
	Printing the elements in queue from front to rear.
	This function is only to test the code.
	This is not a standard function for Queue implementation.
	*/
	void Print();

public:
	// Constructor - set front and rear as -1. 
	// We are assuming that for an empty Queue, both front and rear will be -1.
	ArrayQueue();

	void BlockPush(gpstime_t time, short *sample, size_t length);

	time_and_samples BlockPop();
};

#endif