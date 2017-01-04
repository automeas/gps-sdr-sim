#include"Array_Queue.h"

ArrayQueue::ArrayQueue()
{
	front = -1;
	rear = -1;
}

// To check wheter Queue is empty or not
bool ArrayQueue::IsEmpty()
{
	return (front == -1 && rear == -1);
}

// To check whether Queue is full or not
bool ArrayQueue::IsFull()
{
	return (rear + 1) % MAX_SIZE == front ? true : false;
}

// Inserts an element in queue at rear end
void ArrayQueue::Enqueue(gpstime_t time, short *sample)
{
	if (IsFull())
	{
		cout << "Error: Queue is Full\n";
		return;
	}
	if (IsEmpty())
	{
		front = rear = 0;
	}
	else
	{
		rear = (rear + 1) % MAX_SIZE;
	}
	A[rear].iq_ptr = &buffer[rear][0];
	memcpy_s(A[rear].iq_ptr, FRAME_SIZE, sample, FRAME_SIZE);
	A[rear].time = time;
}

// Removes an element in Queue from front end. 
void ArrayQueue::Dequeue()
{
	cout << "Dequeuing \n";
	if (IsEmpty())
	{
		cout << "Error: Queue is Empty\n";
		return;
	}
	else if (front == rear)
	{
		rear = front = -1;
	}
	else
	{
		front = (front + 1) % MAX_SIZE;
	}
}
// Returns element at front of queue. 
time_and_samples ArrayQueue::Front()
{
	if (front == -1)
	{
		cout << "Error: cannot return front from empty queue\n";
		return A[0]; // dirty fix
	}
	return A[front];
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