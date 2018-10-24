/*
compile with:
	g++ threadex3_orig.cpp -lpthread -o thread3

run with:
	.\thread3
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// initialize mutex to be used to protect conditions
// initialize mutex to protect the variable "count"
// initialize condition

// declare functionCount1 and functionCount2


int  count = 0;

#define COUNT_DONE  10
#define COUNT_HALT1  3
#define COUNT_HALT2  6

void
main()
{
   // declare the ids of thread 1 and thread 2
   // create two independent threads each of which will execute functionC

   // Wait till threads are complete before main continues.

   printf("Final count: %d\n",count);
   exit(0);
}


void *functionCount1()
{
   while(1)
   {
      // Lock mutex

	while (count >= COUNT_HALT1 && count <= COUNT_HALT2)
	{
      // add condition to wait while functionCount2() signals condition
	}
	// Unlock mutex

	// Lock mutex
      count++;
	// Unlock mutex

      printf("Counter value functionCount1: %d\n",count);

       if(count >= COUNT_DONE) return(NULL);
    }
}

void *functionCount2()
{
    while (1)
    {
	// Lock mutex
       if( count < COUNT_HALT1 || count > COUNT_HALT2 )
       {
          // Signal condition to free waiting thread.

 	}
        // Unlock mutex

	// Lock mutex
          count++;
          printf("Counter value functionCount2: %d\n",count);
        // unlock mutex

       if(count >= COUNT_DONE) return(NULL);
    }
}

// Optional Exercise: add a new thread that is allowed to execute only when the second thread has terminated
// its execution
