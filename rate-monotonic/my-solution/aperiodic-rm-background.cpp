// Tre thread periodici e due thread aperiodici
// implementati come background schedule, NON polling server
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>

// application specific code
void task1_code( );
void task2_code( );
void task3_code( );
void task4_code( );
void task5_code( );

// thread functions
void *task1( void *);
void *task2( void *);
void *task3( void *);
void *task4( void *);
void *task5( void *);


pthread_mutex_t mutex_task_4 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_task_5 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond_task_4  = PTHREAD_COND_INITIALIZER;
pthread_cond_t  cond_task_5  = PTHREAD_COND_INITIALIZER;


#define NTASKS 5
#define NPERIODICTASKS 3
#define NAPERIODICTASKS 2

long int periods[NTASKS]; // Periodi dei task, per i task aperiodici questa varabile sarà 0
struct timeval next_arrival_time[NTASKS];
long int WCET[NTASKS]; // Worst case execution time
int missed_deadlines[NTASKS]; // Deadlines mancate dai task periodici
pthread_attr_t attributes[NTASKS]; // Attributi dei thread
sched_param parameters[NTASKS];
pthread_t thread_id[NTASKS]; // Id dei thread

int main() {
  if (getuid() != 0) {
    printf("Must run with root privileges!\n");
    return -1;
  }

  periods[0] = 100000000;
  periods[1] = 200000000;
  periods[2] = 400000000;

  //for aperiodic tasks we set the period equals to 0
  periods[3] = 0;
  periods[4] = 0;

  // Definisco la massima e minima priorità
  sched_param priomax, priomin;
  priomax.sched_priority = sched_get_priority_max(SCHED_FIFO);
  priomin.sched_priority = sched_get_priority_min(SCHED_FIFO);

  if (getuid() == 0)
    pthread_setschedparam(pthread_self(),SCHED_FIFO,&priomax);

  // Eseguo le funzioni dei task per valutarne la schedulabilità
  for (int i =0; i < NTASKS; i++) {
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

    //aperiodic tasks
    if (i==3)
      task4_code();
    if (i==4)
      task5_code();

    gettimeofday(&timeval2, &timezone2);

    WCET[i]= 1000*((timeval2.tv_sec - timeval1.tv_sec)*1000000 + (timeval2.tv_usec-timeval1.tv_usec));
    printf("\nWorst Case Execution Time %d = %d \n", i, WCET[i]);
  }

  // Calcolo U (Utilization factor) e Ulub
  double U = 0;
  for (int i = 0; i < NPERIODICTASKS; i++)
    U+= ((double)WCET[i])/((double)periods[i]);

  double Ulub = NPERIODICTASKS*(pow(2.0,(1.0/NPERIODICTASKS)) -1);

  if (U > Ulub) {
    printf("\n U = %lf Ulub = %lf Not schedulable\n", U, Ulub);
    return(-1);
  }

  printf("\n U = %lf Ulub = %lf Schedulable\n", U, Ulub);
  fflush(stdout);
  sleep(5);

  if (getuid() == 0)
    pthread_setschedparam(pthread_self(),SCHED_FIFO,&priomin);

  // Setto gli attributi per i task perdiodici
  for (int i =0; i < NPERIODICTASKS; i++) {
    pthread_attr_init(&(attributes[i]));
    pthread_attr_setinheritsched(&(attributes[i]), PTHREAD_EXPLICIT_SCHED);

    pthread_attr_setschedpolicy(&(attributes[i]), SCHED_FIFO);

    parameters[i].sched_priority = priomax.sched_priority - i;

    pthread_attr_setschedparam(&(attributes[i]), &(parameters[i]));
  }

  // aperiodic tasks
  for (int i = NPERIODICTASKS; i < NTASKS; i++) {
    pthread_attr_init(&(attributes[i]));
    pthread_attr_setschedpolicy(&(attributes[i]), SCHED_FIFO);

    //set minimum priority (background scheduling)
    parameters[i].sched_priority = 1; // 0;
    pthread_attr_setschedparam(&(attributes[i]), &(parameters[i]));
  }

  int iret[NTASKS];
  struct timeval now;
  struct timezone zone;
  gettimeofday(&now, &zone);

  for (int i = 0; i < NPERIODICTASKS; i++) {
    long int periods_micro = periods[i]/1000;
    next_arrival_time[i].tv_sec  = now.tv_sec  + periods_micro/1000000;
    next_arrival_time[i].tv_usec = now.tv_usec + periods_micro%1000000;
    missed_deadlines[i] = 0;
  }

  // thread creation
  iret[0] = pthread_create( &(thread_id[0]), &(attributes[0]), task1, NULL);
  iret[1] = pthread_create( &(thread_id[1]), &(attributes[1]), task2, NULL);
  iret[2] = pthread_create( &(thread_id[2]), &(attributes[2]), task3, NULL);
  iret[3] = pthread_create( &(thread_id[3]), &(attributes[3]), task4, NULL);
  iret[4] = pthread_create( &(thread_id[4]), &(attributes[4]), task5, NULL);

  // Attendo solo i task periodici perchè se aspettassi quelli periodici che di
  // fatto non terminano mai non terminerebbe
  pthread_join( thread_id[0], NULL);
  pthread_join( thread_id[1], NULL);
  pthread_join( thread_id[2], NULL);


  exit(0);
}

void task1_code() {
  double uno;
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 1000; j++) {
      uno = rand()*rand()%10;
    }
  }
  printf("1");

  if (uno == 0) {
    printf(":Execute (4)");fflush(stdout);
    pthread_mutex_lock(&mutex_task_4);
    pthread_cond_signal(&cond_task_4);
    pthread_mutex_unlock(&mutex_task_4);
  }

  if (uno == 1) {
    printf(":Execute (5)");fflush(stdout);
    pthread_mutex_lock(&mutex_task_5);
    pthread_cond_signal(&cond_task_5);
    pthread_mutex_unlock(&mutex_task_5);
  }

  fflush(stdout);
}

void *task1(void *) {
  struct timespec waittime;
  waittime.tv_sec=0; /* seconds */
  waittime.tv_nsec = periods[0]; /* nanoseconds */

  for (int i=0; i < 100; i++) {
    task1_code();

    struct timeval ora;
    struct timezone zona;
    gettimeofday(&ora, &zona);

    long int timetowait= 1000*((next_arrival_time[0].tv_sec - ora.tv_sec)*1000000
			 +(next_arrival_time[0].tv_usec-ora.tv_usec));

    if (timetowait <0) missed_deadlines[0]++;

    waittime.tv_sec = timetowait/1000000000;
    waittime.tv_nsec = timetowait%1000000000;

    nanosleep(&waittime, NULL);

    long int periods_micro=periods[0]/1000;
    next_arrival_time[0].tv_sec = next_arrival_time[0].tv_sec + periods_micro/1000000;
    next_arrival_time[0].tv_usec = next_arrival_time[0].tv_usec + periods_micro%1000000;
  }
}


void task2_code() {
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 1000; j++){
	     double uno = rand()*rand();
     }
  }
  printf("2");
  fflush(stdout);
}


void *task2( void *ptr ) {
  struct timespec waittime;
  waittime.tv_sec = 0; /* seconds */
  waittime.tv_nsec = periods[1]; /* nanoseconds */
  for (int i=0; i < 100; i++) {
    task2_code();
    struct timeval ora;
    struct timezone zona;
    gettimeofday(&ora, &zona);
    long int timetowait= 1000*((next_arrival_time[1].tv_sec -
			  ora.tv_sec)*1000000 +(next_arrival_time[1].tv_usec-ora.tv_usec));

    if (timetowait <0) missed_deadlines[1]++;

    waittime.tv_sec = timetowait/1000000000;
    waittime.tv_nsec = timetowait%1000000000;

    nanosleep(&waittime, NULL);

    long int periods_micro=periods[1]/1000;
    next_arrival_time[1].tv_sec = next_arrival_time[1].tv_sec + periods_micro/1000000;
    next_arrival_time[1].tv_usec = next_arrival_time[1].tv_usec + periods_micro%1000000;
  }
}

void task3_code() {
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 1000; j++);
      double uno = rand()*rand();
  }
  printf("3");
  fflush(stdout);
}

void *task3( void *) {
  int i=0;
  struct timespec waittime;
  waittime.tv_sec=0; /* seconds */
  waittime.tv_nsec = periods[2]; /* nanoseconds */
  for (i=0; i < 100; i++) {
    task3_code();
    struct timeval ora;
    struct timezone zona;
    gettimeofday(&ora, &zona);
    long int timetowait= 1000*((next_arrival_time[2].tv_sec - ora.tv_sec)*1000000
			 +(next_arrival_time[2].tv_usec-ora.tv_usec));

    if (timetowait <0) missed_deadlines[2]++;

    waittime.tv_sec = timetowait/1000000000;
    waittime.tv_nsec = timetowait%1000000000;

    nanosleep(&waittime, NULL);

    long int periods_micro=periods[2]/1000;
    next_arrival_time[2].tv_sec = next_arrival_time[2].tv_sec + periods_micro/1000000;
    next_arrival_time[2].tv_usec = next_arrival_time[2].tv_usec + periods_micro%1000000;
  }
}


// Task aperiodici

void task4_code() {
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 1000; j++) {
	     double uno = rand()*rand();
    }
  }
  printf(" -aperiodic 4- ");
  fflush(stdout);
}

void *task4( void *) {
  while (1) {
    // waiting for task 1 to signal the condition
    pthread_mutex_lock(&mutex_task_4);
    pthread_cond_wait(&cond_task_4, &mutex_task_4);
    pthread_mutex_unlock(&mutex_task_4);
    task4_code();
  }
}

void task5_code() {
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 1000; j++) {
	     double uno = rand()*rand();
    }
  }
  printf(" -aperiodic 5- ");
  fflush(stdout);
}

void *task5( void *) {
  while(1) {
    pthread_mutex_lock(&mutex_task_5);
    pthread_cond_wait(&cond_task_5, &mutex_task_5);
    pthread_mutex_unlock(&mutex_task_5);
    task5_code();
  }
}
