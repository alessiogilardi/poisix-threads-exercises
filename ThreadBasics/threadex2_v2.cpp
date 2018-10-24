/*

  g++ threadex2_v2.cpp -lpthread -o thread2_v2
*/


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *functionC(void *ptr);

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int  counter = 0;
int temp;

int main() {
  int threads_num = 10;
  int *irets = new int[threads_num];
  int *rets = new int[threads_num];
  pthread_t *threads = new pthread_t[threads_num];

  for (int i = 0; i < threads_num; i++)
  {
    if (irets[i] = pthread_create(&threads[i], NULL, functionC, NULL))
    {
      printf("Thread %d creation failed: %d\n", i, irets[i]);
    }
  }

  for (int i = 0; i < threads_num; i++)
  {
    pthread_join(threads[i], (void **) &rets[i]);
  }

  for (int i = 0; i < threads_num; i++)
  {
    printf("Thread %d returns: %d\n", i, rets[i]);
  }

  exit(0);
}

void *functionC(void *ptr)
{
  pthread_mutex_lock(&mutex1);
  int temp = counter;
  temp = temp +1;
  counter = temp;
  printf("%d\n", counter);
  pthread_mutex_unlock(&mutex1);

  pthread_exit((void *) counter);
}
