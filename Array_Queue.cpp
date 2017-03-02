#include"Array_Queue.h"
ArrayQueue::ArrayQueue()
{
	front = 0;
	rear = 0;
}

// To check wheter Queue is empty or not
bool ArrayQueue::IsEmpty()
{
	return (front == rear ? true : false);
}

// To check whether Queue is full or not
bool ArrayQueue::IsFull()
{
	return (rear + 1) % MAX_SIZE == front ? true : false;
}

// Inserts an element in queue at rear end
void ArrayQueue::Enqueue(gpstime_t time, short *sample, size_t length)
{
	if (IsFull())
	{
		cout << "Error: Queue is Full\n";
		return;
	}
	else
	{
		A[rear].iq_ptr = &buffer[rear][0];
		memcpy_s(A[rear].iq_ptr, length, sample, FRAME_SIZE);
		A[rear].time = time;
		rear = (rear + 1) % MAX_SIZE;
	}
}

// Removes an element in Queue from front end. 
void ArrayQueue::Dequeue()
{
	// cout << "Dequeuing \n";
	if (IsEmpty())
	{
		cout << "Error: Queue is Empty\n";
		return;
	}
	else
	{
		front = (front + 1) % MAX_SIZE;
	}
}
/*
Printing the elements in queue from front to rear.
This function is only to test the code.
This is not a standard function for Queue implementation.
*/
void ArrayQueue::Print()
{
	// Finding number of elements in queue  
	int count = (rear + MAX_SIZE - front) % MAX_SIZE + 1;
	cout << "ArrayQueue       : ";
	for (int i = 0; i <count; i++)
	{
		int index = (front + i) % MAX_SIZE; // Index of element while travesing circularly from front
		cout << A[index].time.sec << " ";
	}
	cout << "\n\n";
}
//int main()
//{
//	/*Driver Code to test the implementation
//	Printing the elements in Queue after each Enqueue or Dequeue
//	*/
//	Queue Q; // creating an instance of Queue. 
//	Q.Enqueue(2);  Q.Print();
//	Q.Enqueue(4);  Q.Print();
//	Q.Enqueue(6);  Q.Print();
//	Q.Dequeue();	  Q.Print();
//	Q.Enqueue(8);  Q.Print();
//	return 0;
//}

void ArrayQueue::BlockPush(gpstime_t time, short *sample, size_t length)
{
	while (IsFull())
	{
		boost::thread::yield(); // queue is full
	}
	//m_lock.lock();
	Enqueue(time, sample, length);
	//m_lock.unlock();
}

time_and_samples ArrayQueue::BlockPop()
{
	static time_and_samples temp_value;
	while (IsEmpty())
	{
		boost::thread::yield(); // queue is empty
	}
	//m_lock.lock();
	temp_value = A[front];
	Dequeue();
	//m_lock.unlock();
	return temp_value;
}
