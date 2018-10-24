//This exercise show how to schedule threads with Rate Monotonic

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <sys/types.h>

//Declaration of functions that contains the application code of each task
//and thread functions 
//Assume that we have 3 tasks with periods 100 ms, 200 ms, 400 ms

void task1_code( );
void *task1( void *);

//Declaration of arrays that can be used for:
// task periods
// arrival time in the following period
// computational time of each task
// scheduling attributes
// thread identifiers
// scheduling parameters
// number of missed deadline


main()
{
  // set task periods in nanoseconds

  // declare two variables priomin and priomax that can be used 
  // to store the maximum and minimum priority

  // set the maximum priority to the current thread (you are required to be
  // superuser)

  // execute all tasks in standalone modality in order to measure execution times
  // (use gettimeofday). Use the computed values to update the worst case execution
  // time of each task.

  // compute Ulub
  // compute U
  // verify schedulability

  // set the minimum priority to the current thread

  // set the attributes of each task, including scheduling policy and priority


  // read the current time (gettimeofday) and set the next arrival time for each task 
  // (i.e., the beginning of the next period)

  // create all threads
 
  // join all threads

}

// application specific task_1 code

void task1_code()
{
   int i,j;
  for (i = 0; i < 10; i++)
    {
      for (j = 0; j < 10000; j++)
	{
		double uno = rand()*rand();
    	}
  }
  printf("1");
  fflush(stdout);
}

//thread code for task_1 (used only for temporization)

void *task1( void *ptr)
{

  //one hundred times... it should be an infinite loop (too dangerous)

  for (i=0; i < 100; i++)
    {
	// execute application specific code

	// after execution, compute the time to the beginning of the next period
      
        // suspend the task until the beginning of the next period
	// (use nanosleep)
     
	// the task is ready: set the next arrival time for each task
	// (i.e., the beginning of the next period)

    }
}
