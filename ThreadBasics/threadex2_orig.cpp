/*
compile with:
	g++ threadex2_orig.cpp -lpthread -o thread2

run with:
	.\thread2
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *functionC(void *ptr);
//void *functionC_2(void *ptr);


pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int  counter = 0;
int temp;

int main()
{
   int rc1, rc2, rc3;
   pthread_t thread1, thread2, thread3;

   if (rc1 = pthread_create(&thread1, NULL, functionC, NULL))
   {
     printf("Trhead 1 creation failed: %d\n", rc1);
   }

   if (rc2 = pthread_create(&thread2, NULL, functionC, NULL))
   {
     printf("Trhead 2 creation failed: %d\n", rc2);
   }


   pthread_join(thread1, (void **) &rc1);
   pthread_join(thread2, (void **) &rc2);

   printf("Thread 1 returns: %d\n", rc1);
   printf("Thread 2 returns: %d\n", rc2);

   exit(0);
}

void *functionC(void *ptr)
{
  int i;
  for (i = 0; i < 10; i++)
  {
    pthread_mutex_lock(&mutex1);
    int temp = counter;
    temp = temp +1;
    counter = temp;
    printf("%d\n", counter);
    pthread_mutex_unlock(&mutex1);
  }

  pthread_exit((void *) counter);
}

// Optional exercise: try to define the mutex as a "recursive" (see the slides about the concept of a "recursive mutex"). Each
// thread must call the lock function and the unlock function twice to test the mechanism.

// Optional exercise: try to use different versions of the lock function: timed_lock, trylock
