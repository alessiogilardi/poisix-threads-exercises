//This exercise show how to schedule threads with Rate Monotonic

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <sys/types.h>

void task1_code( );
void task2_code( );
void task3_code( );
void *task1( void *);
void *task2( void *);
void *task3( void *);

/**
 * Quando U supera Ulub non significa che non siano schedulabili ma solo che
 * la condizioe sufficiente non è rispettata, possono comunque essere schedulabili
 */

#define NTASKS 3
long int periods[NTASKS];
struct timeval next_arrival_time[NTASKS];
long int WCET[NTASKS]; // Worst case execution time
                       // --> tempo di esecuzione nel caso peggiore usato per calcolare U = sum (Ui/Ti)
pthread_attr_t attributes[NTASKS];
pthread_t thread_id[NTASKS];
struct sched_param parameters[NTASKS];
int missed_deadlines[NTASKS];

int main()
{
  // set task periods in nanoseconds

  periods[0]= 100000000; //in nanosecondi
  periods[1]= 200000000; //in nanosecondi
  periods[2]= 400000000; //in nanosecondi

  struct sched_param priomax;
  priomax.sched_priority=sched_get_priority_max(SCHED_FIFO);
  struct sched_param priomin;
  priomin.sched_priority=sched_get_priority_min(SCHED_FIFO);

  // set the maximum priority to the current thread (you are required to be
  // superuser

  if (getuid() == 0)
    pthread_setschedparam(pthread_self(),SCHED_FIFO,&priomax);

  // execute all tasks in standalone modality in order to measure execution times
  // (use gettimeofday). Use the computed values to update the worst case execution
  // time of each task.

  int i;
  for (i =0; i < NTASKS; i++)
    {
      struct timeval timeval1;
      struct timezone timezone1;
      struct timeval timeval2;
      struct timezone timezone2;
      gettimeofday(&timeval1, &timezone1);

      if (i==0)
	task1_code();
      if (i==1)
	task2_code();
      if (i==2)
	task3_code();
      gettimeofday(&timeval2, &timezone2);


      WCET[i]= 1000*((timeval2.tv_sec - timeval1.tv_sec)*1000000
			       +(timeval2.tv_usec-timeval1.tv_usec));
      printf("\nWorst Case Execution Time %d=%d \n", i, WCET[i]);
    }

  // compute U
	double U = WCET[0]/periods[0]+WCET[1]/periods[1]+WCET[2]/periods[2];
  // compute Ulub --> Ulub può essere 1 perchè i periodi sono in relazione armonica tra di loro
	double Ulub = 1;

  if (U > Ulub)
    {
      printf("\n U=%lf Ulub=%lf Non schedulable Task Set", U, Ulub);
      return(-1);
    }
  printf("\n U=%lf Ulub=%lf Scheduable Task Set", U, Ulub);
  fflush(stdout);
  sleep(5);

  // set the minimum priority to the current thread

  if (getuid() == 0)
    pthread_setschedparam(pthread_self(),SCHED_FIFO,&priomin);


  // set the attributes of each task, including scheduling policy and priority
  for (i =0; i < NTASKS; i++)
    {
      pthread_attr_init(&(attributes[i]));

      pthread_attr_setinheritsched(&(attributes[i]), PTHREAD_EXPLICIT_SCHED);
      pthread_attr_setschedpolicy(&(attributes[i]), SCHED_FIFO);

      parameters[i].sched_priority = priomin.sched_priority+NTASKS - i;
      pthread_attr_setschedparam(&(attributes[i]), &(parameters[i]));
    }
  int iret[NTASKS];
  struct timeval ora;
  struct timezone zona;
  gettimeofday(&ora, &zona);

  // read the current time (gettimeofday) and set the next arrival time for each task
  // (i.e., the beginning of the next period)

  for (i = 0; i < NTASKS; i++)
    {
      long int periods_micro = periods[i]/1000;
      next_arrival_time[i].tv_sec = ora.tv_sec + periods_micro/1000000;
      next_arrival_time[i].tv_usec = ora.tv_usec + periods_micro%1000000;
      missed_deadlines[i] = 0;
    }
  // create all threads

  iret[0] = pthread_create( &(thread_id[0]), &(attributes[0]), task1, NULL);
  iret[1] = pthread_create( &(thread_id[1]), &(attributes[1]), task2, NULL);
  iret[2] = pthread_create( &(thread_id[2]), &(attributes[2]), task3, NULL);


  // join all threads
  pthread_join( thread_id[0], NULL);
  pthread_join( thread_id[1], NULL);
  pthread_join( thread_id[2], NULL);
  exit(0);
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
  int i=0;
  struct timespec waittime;
  waittime.tv_sec=0; /* seconds */
  waittime.tv_nsec = periods[0]; /* nanoseconds */



   /* Codice per macchine multi core da eseguire in ogni thread */
   /*
   cpu_set_t cset;
   CPU_ZERO( &cset );
   CPU_SET( 0, &cset);

   pthread_setaffinity_np( pthread_self(), sizeof(cpu_set_t), &cset);
   */

   //one hundred times... it should be an infinite loop (too dangerous
  for (i=0; i < 100; i++)
    {
      // execute application specific code
	task1_code();

      struct timeval ora;
      struct timezone zona;
      gettimeofday(&ora, &zona);

      // after execution, compute the time to the beginning of the next period
      long int timetowait= 1000*((next_arrival_time[0].tv_sec - ora.tv_sec)*1000000
				 +(next_arrival_time[0].tv_usec-ora.tv_usec));
      if (timetowait <0)
	missed_deadlines[0]++;

      waittime.tv_sec = timetowait/1000000000;
      waittime.tv_nsec = timetowait%1000000000;

      // suspend the task until the beginning of the next period
      // (use nanosleep)

      nanosleep(&waittime, NULL);

      // the task is ready: set the next arrival time for each task
      // (i.e., the beginning of the next period)

      long int periods_micro=periods[0]/1000;
      next_arrival_time[0].tv_sec = next_arrival_time[0].tv_sec +
	periods_micro/1000000;
      next_arrival_time[0].tv_usec = next_arrival_time[0].tv_usec +
	periods_micro%1000000;
    }
}
void task2_code()
{
	int i;
  for (i = 0; i < 10; i++)
    {
	int j;
      for (j = 0; j < 10000; j++)
	{
	double uno = rand()*rand();
	}
    }
  printf("2");
  fflush(stdout);
}


void *task2( void *ptr )
{
  int i=0;
  struct timespec waittime;
  waittime.tv_sec=0; /* seconds */
  waittime.tv_nsec = periods[1]; /* nanoseconds */
  for (i=0; i < 100; i++)
    {
      task2_code();
      struct timeval ora;
      struct timezone zona;
      gettimeofday(&ora, &zona);
      long int timetowait= 1000*((next_arrival_time[1].tv_sec -
				  ora.tv_sec)*1000000 +(next_arrival_time[1].tv_usec-ora.tv_usec));
      if (timetowait <0)
	missed_deadlines[1]++;
      waittime.tv_sec = timetowait/1000000000;
      waittime.tv_nsec = timetowait%1000000000;
      nanosleep(&waittime, NULL);
      long int periods_micro=periods[1]/1000;
      next_arrival_time[1].tv_sec = next_arrival_time[1].tv_sec + periods_micro/1000000;
      next_arrival_time[1].tv_usec = next_arrival_time[1].tv_usec +
	periods_micro%1000000;
    }
}
void task3_code()
{
	int i;
  for (i = 0; i < 10; i++)
    {
	int j;
      for (j = 0; j < 10000; j++);
      double uno = rand()*rand();
    }
  printf("3");
  fflush(stdout);
}

void *task3( void *ptr)
{
  int i=0;
  struct timespec waittime;
  waittime.tv_sec=0; /* seconds */
  waittime.tv_nsec = periods[2]; /* nanoseconds */
  for (i=0; i < 100; i++)
    {
      task3_code();
      struct timeval ora;
      struct timezone zona;
      gettimeofday(&ora, &zona);
      long int timetowait= 1000*((next_arrival_time[2].tv_sec - ora.tv_sec)*1000000
				 +(next_arrival_time[2].tv_usec-ora.tv_usec));
      if (timetowait <0)
	missed_deadlines[2]++;
      waittime.tv_sec = timetowait/1000000000;
      waittime.tv_nsec = timetowait%1000000000;
      nanosleep(&waittime, NULL);
      long int periods_micro=periods[2]/1000;
      next_arrival_time[2].tv_sec = next_arrival_time[2].tv_sec +
	periods_micro/1000000;
      next_arrival_time[2].tv_usec = next_arrival_time[2].tv_usec +
	periods_micro%1000000;
    }
}
