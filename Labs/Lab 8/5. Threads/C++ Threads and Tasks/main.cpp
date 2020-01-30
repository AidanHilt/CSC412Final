//
//  main.cpp
//  Threads and Tasks
//
//  Created by Jean-Yves Hervé [Local] on 2016-04-27.
//  Copyright © 2016 Jean-Yves Hervé. All rights reserved.
//

#define VERSION		4

#if VERSION == 1

	//--------------------------------
	//	new style pthreads (no param)  C++11
	//--------------------------------

	#include <iostream>
	#include <thread>
	#include <unistd.h>

	void threadFunc(void)
	{
		std::cout << "In the thread function" << std::endl;
		
		//	do stuff
		usleep(2000000);

		std::cout << "Thread terminated" << std::endl;
	}

	int main()
	{
		std::thread thr(threadFunc);

		std::cout << "Thread launched from main\n";
		
		//	do stuff

		//	Wait for the thread to terminate
		thr.join();
		std::cout << "Thread joined in main\n";

		return 0;
	}

#elif VERSION == 2

	//--------------------------------
	//	new style pthreads (with param)  C++11
	//--------------------------------

	#include <iostream>
	#include <thread>
	#include <unistd.h>

	void threadFunc(int val)
	{
		std::cout << "In the thread function, received val=" << val << std::endl;
		
		//	do stuff
		usleep(2000000);

		std::cout << "Thread terminated" << std::endl;
	}

	int main()
	{
		std::thread thr(threadFunc, 5);
		//	argh!
		//		std::thread thr(threadFunc);

		std::cout << "Thread launched from main\n";
		
		//	do stuff

		//	Wait for the thread to terminate
		thr.join();
		std::cout << "Thread joined in main\n";

		return 0;
	}

#elif VERSION == 3

	//----------------------------------------------
	//	new style pthreads (with i/o param)  C++11
	//----------------------------------------------

	#include <iostream>
	#include <thread>
	#include <vector>
	#include <unistd.h>

	using DataStruct = struct {
					int param1;
					float param2;
					float param3;
					int return1;
					std::vector<float> return2;
	} ;

	void threadFunc(DataStruct* data) noexcept;


	void threadFunc(DataStruct* data) noexcept
	{
		std::cout << "In the thread function" << std::endl;
		
		std::cout	<< "Received data struct with param1 = " << data->param1
					<< ", param2 = " << data->param2
					<< ", param3 = " << data->param3 << std::endl;
		
		//	do stuff
		usleep(2000000);
		
		data->return1 = 2*data->param1;
		data->return2 = std::vector<float>(data->return1);
		for (int k=0; k<data->return1; k++)
			data->return2[k] = 1000.f*data->param2 + k*data->param3;
		
		//	noting to return
		
		std::cout << "The thread terminated" << std::endl;
	}

//	void anotherThreadFunc(const DataStruct& data)
//	{
//		std::cout << "In the thread function" << std::endl;
//		
//		std::cout	<< "Received data struct with param1 = " << data.param1
//					<< ", param2 = " << data.param2
//					<< ", param3 = " << data.param3 << std::endl;
//		
//		//	do stuff
//		
////		data.return1 = 2*data.param1;
////		data.return2 = std::vector<float>(data.return1);
////		for (int k=0; k<data.return1; k++)
////			data.return2[k] = 1000.f*data.param1 + k*data.param2;
////		
//		//	noting to return
//		
//		std::cout << "The thread terminated" << std::endl;
//	}

	int main()
	{
		DataStruct myData = {	4, 2.5f, 12.5f	//	params 1, 2, 3
								};
		std::thread thr(threadFunc, &myData);
//		std::thread thr2(anotherThreadFunc, myData);

		std::cout << "Thread launched from main\n";
		
		//	do stuff

		//	Wait for the thread to terminate
		thr.join();
		
		std::cout << "Joined in main thread, got data back:" << std::endl;
		std::cout << "\treturn1:\t" << myData.return1 << std::endl;
		std::cout << "\treturn2:\t";
		for (int k=0; k<myData.return2.size(); k++)
		{
			if (k>0)
				std::cout << "\t\t\t";
			std::cout << myData.return2[k] << std::endl;
		}

		return 0;
	}

	//	Problem with direct creation of software threads:  limited resource
	//	If too many software threads are created --> std::system_error thrown
	//	[even if the thread's function is noexcept]

#elif VERSION == 4

	//----------------------------------------------
	//	new style task C++14 [adapted from Cplusplus.com]
	//----------------------------------------------

	// launch::async vs launch::deferred
	#include <iostream>     // std::cout
	#include <future>       // std::async, std::deferred std::future, std::launch
	#include <chrono>       // std::chrono::milliseconds
	#include <thread>       // std::this_thread::sleep_for
	#include <unistd.h>

	void print_ten (char c, int ms)
	{
	  for (int i=0; i<10; ++i)
	  {
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));
		std::cout << c;
	  }
		std::cout << "Task terminated\n";
	}

	int main ()
	{
		//	I separated declaration from initialization so that the first
		//	launch does not appear (unfairly) more complex than the others.
		std::future<void> foo;
		std::future<void> bar;

		std::cout << "with explicit launch::async:\n";
		foo = std::async(std::launch::async, print_ten, '*', 100);
		bar = std::async(std::launch::async, print_ten, '@', 200);

		// async "get" (wait for foo and bar to be ready):
		bar.get();
		foo.get();
		std::cout << "\n\n";

		std::cout << "with explicit launch::deferred:\n";
		foo = std::async(std::launch::deferred, print_ten, '*', 100);
		bar = std::async(std::launch::deferred, print_ten, '@', 200);
		// deferred "get" (perform the actual calls):
		foo.get();
		bar.get();
		std::cout << "\n\n";

		std::cout << "with default launch:\n";
		foo = std::async(print_ten, '*', 100);
		bar = std::async(print_ten, '@', 200);
		// deferred "get" (perform the actual calls):
		foo.get();
		bar.get();
		std::cout << "\n\n";

//
//		//	This is where auto comes handy to make code readable
//		std::cout << "the same, with auto declaration:\n";
//		auto foo2 = std::async(print_ten, '*', 100);
//		auto bar2 = std::async(print_ten, '@', 200);
//		// deferred "get" (perform the actual calls):
//		foo2.get();
//		bar2.get();
//		std::cout << "\n\n";

		return 0;
	}

#elif VERSION == 5

	//----------------------------------------------
	//	new style task, with return value C++14
	//----------------------------------------------

	// launch::async vs launch::deferred
	#include <iostream>     // std::cout
	#include <future>       // std::async, std::future, std::launch
	#include <chrono>       // std::chrono::milliseconds
	#include <thread>
	#include <vector>

	//	silly function
	template <typename T>
	std::vector<T> makePalindrome(const std::vector<T>& vec)
	{
		std::vector<T> outVec;
		for (int k=0; k<vec.size(); k++)
		{
			outVec.push_back(vec[k]);
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
		}
		for (int k=vec.size()-1; k>=0; k--)
		{
			outVec.push_back(vec[k]);
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
		}
		
		return outVec;
	}

	int main ()
	{
		std::vector<int>v1 {12, 34, 35, 22};

		std::cout << "Creating an explicitly asynchronous task" << std::endl;

		auto task1 = std::async(std::launch::async, makePalindrome<int>, v1);
		std::cout << "Waiting in the main" << std::flush << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << "End wait" << std::flush << std::endl;

		auto out1 = task1.get();
		std::cout << "Values returned by task 1: ";
		for(auto val : out1)
		{
			std::cout << val << "  ";
		}
		std::cout << "\n\n";
		
		std::cout << "Creating an explicitly deferred task" << std::endl;

		auto task2 = std::async(std::launch::deferred, makePalindrome<int>, v1);
		std::cout << "Waiting in the main" << std::flush << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << "End wait" << std::flush << std::endl;
		

		auto out2 = task2.get();
		std::cout << "Values returned by task 2: ";
		for(auto val : out2)
		{
			std::cout << val << "  ";
		}
		std::cout << "\n\n";
		
		
		
		return 0;
	}

#else
	#error invalid version number
#endif
