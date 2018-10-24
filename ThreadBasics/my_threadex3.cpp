/*
compile with:
	g++ my_threadex3.cpp -lpthread -o my_thread3
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// initialize mutex to be used to protect conditions
// initialize mutex to protect the variable "count"
// initialize condition

pthread_mutex_t count_mutex     = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t condition_var    = PTHREAD_COND_INITIALIZER;

// declare functionCount1 and functionCount2
void *functionCount1(void *ptr);
void *functionCount2(void *ptr);
void *functionCount2_v2(void *ptr);
void *functionCount3(void *ptr);

int  count = 0;

#define COUNT_DONE  10
#define COUNT_HALT1  3
#define COUNT_HALT2  6

int main()
{
   // declare the ids of thread 1 and thread 2
   // create two independent threads each of which will execute functionC

   // Wait till threads are complete before main continues.
   int threads_num = 3;
   int *irets = new int[threads_num];
   int *rets  = new int[threads_num];

   pthread_t *threads = new pthread_t[threads_num];


   if (irets[0] = pthread_create(&threads[0], NULL, functionCount1, NULL))
   {
     printf("Thread %d creation failed: %d\n", 0, irets[0]);
     exit(-1);
   }
/*
   if (irets[1] = pthread_create(&threads[1], NULL, functionCount2, NULL))
   {
     printf("Thread %d creation failed: %d\n", 1, irets[1]);
     exit(-1);
   }
*/
  if (irets[1] = pthread_create(&threads[1], NULL, functionCount2_v2, NULL))
  {
    printf("Thread %d creation failed: %d\n", 1, irets[1]);
    exit(-1);
  }

 if (irets[2] = pthread_create(&threads[2], NULL, functionCount3, NULL))
 {
   printf("Thread %d creation failed: %d\n", 2, irets[2]);
   exit(-1);
 }


  for (int i = 0; i < threads_num; i++)
  {
    pthread_join(threads[i], NULL);
  }

  printf("Final count: %d\n", count);
  exit(0);
}


void *functionCount1(void *ptr)
{
  while (1)
  {
    pthread_mutex_lock(&condition_mutex);
    while (count >= COUNT_HALT1 && count <= COUNT_HALT2)
    {
      // waiting
      pthread_cond_wait(&condition_var, &condition_mutex);
    }
    pthread_mutex_unlock(&condition_mutex);

    pthread_mutex_lock(&count_mutex);
    count++;
    printf("Counter value functionCount1: %d\n", count);
    pthread_mutex_unlock(&count_mutex);

    if(count >= COUNT_DONE) return NULL;
  }
}

void *functionCount2(void *ptr)
{
  while (1)
  {
    pthread_mutex_lock(&condition_mutex);
    if( count < COUNT_HALT1 || count > COUNT_HALT2 )
    {
       // Signal condition to free waiting thread.
       pthread_cond_signal(&condition_var);
    }
    pthread_mutex_unlock(&condition_mutex);

    pthread_mutex_lock(&count_mutex);
    count++;
    printf("Counter value functionCount2: %d\n", count);
    pthread_mutex_unlock(&count_mutex);

    if(count >= COUNT_DONE) return NULL;
  }
}

void *functionCount2_v2(void *ptr)
{
  while (1)
  {
    pthread_mutex_lock(&condition_mutex);
    if( count < COUNT_HALT1 || count > COUNT_HALT2 )
    {
       // Signal condition to free waiting thread.
       pthread_cond_signal(&condition_var);
    }
    pthread_mutex_unlock(&condition_mutex);

    pthread_mutex_lock(&count_mutex);
    count++;
    printf("Counter value functionCount2_v2: %d\n", count);
    pthread_mutex_unlock(&count_mutex);


    if(count >= COUNT_DONE)
    {
      pthread_mutex_lock(&condition_mutex);
      pthread_cond_signal(&condition_var);
      pthread_mutex_unlock(&condition_mutex);
      return NULL;
    }
  }
}

void *functionCount3(void *ptr)
{
  pthread_mutex_lock(&condition_mutex);
  while (count < COUNT_DONE)
  {
    // waiting
    pthread_cond_wait(&condition_var, &condition_mutex);
  }
  pthread_mutex_unlock(&condition_mutex);


  pthread_mutex_lock(&count_mutex);
  count++;
  printf("Counter value functionCount3: %d\n", count);
  pthread_mutex_unlock(&count_mutex);

  return NULL;
}

// Optional Exercise: add a new thread that is allowed to execute only when the second thread has terminated
// its execution
