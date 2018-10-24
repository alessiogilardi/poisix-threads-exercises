#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <sys/types.h>

/**
 * Quando U supera Ulub non significa che non siano schedulabili ma solo che
 * la condizioe sufficiente non è rispettata, possono comunque essere schedulabili
 */

void task1_code();
void task2_code();
void task3_code();

void *task1(void *ptr);
void *task2(void *ptr);
void *task3(void *ptr);

#define NTASKS 3

long int periods[NTASKS];
struct timeval next_arrival_time[NTASKS];
long int WCET[NTASKS]; // Worst case execution time --> tempo di esecuzione nel caso peggiore usato per calcolare U = sum (Ui/Ti)
pthread_attr_t attributes[NTASKS];
pthread_t thread_id[NTASKS];
struct sched_param parameters[NTASKS]; // Parametri per la schedulazione
int missed_deadlines[NTASKS]; // Array per tenere traccia di quante volte un task manca la deadline

int main() {

  if (getuid() != 0) {
    printf("Devi essere root\n");
    exit(-1);
  }

  periods[0] = 100000000; //in nanosecondi
  periods[1] = 200000000; //in nanosecondi
  periods[2] = 400000000; //in nanosecondi

  struct sched_param priomax;
  struct sched_param priomin;
  priomax.sched_priority = sched_get_priority_max(SCHED_FIFO); // Setto priomax come la massima priorità
  priomin.sched_priority = sched_get_priority_min(SCHED_FIFO); // Setto priomin come la minima priorità

  // Controllo di essere root altrimenti non sarò in grado di settare
  // la massima priorità per i miei thread
  if (getuid() == 0)
    pthread_setschedparam(pthread_self(), SCHED_FIFO, &priomax);

  // Execute all tasks in standalone modality in order to measure execution times
  // (use gettimeofday). Use the computed values to update the worst case execution
  // time of each task.

  for (int i = 0; i < NTASKS; i++) {
    struct timeval timeval1;
    struct timezone timezone1;
    struct timeval timeval2;
    struct timezone timezone2;

    gettimeofday(&timeval1, &timezone1);
    if (i == 0)
      task1_code();
    if (i == 1)
      task2_code();
    if (i == 2)
      task3_code();
    gettimeofday(&timeval2, &timezone2);

    WCET[i] = 1000*((timeval2.tv_sec - timeval1.tv_sec)*1000000 + (timeval2.tv_usec-timeval1.tv_usec));
    printf("\nWorst Case Execution Time %d=%d \n", i, WCET[i]);
  }

  // compute U
  double U = 0;
  for (int i = 0; i < NTASKS; i++) {
    U += (double) WCET[i]/periods[i];
  }
	//double U = (double)WCET[0]/periods[0] + (double)WCET[1]/periods[1] + (double)WCET[2]/periods[2];

  // compute Ulub --> Ulub può essere 1 perchè i periodi sono in relazione armonica tra di loro
	double Ulub = 1;
  if (U > Ulub) {
      printf("\n U=%lf Ulub=%lf Non schedulable Task Set\n", U, Ulub);
      return(-1);
  }
  printf("\n U=%lf Ulub=%lf Scheduable Task Set\n\n", U, Ulub);
  fflush(stdout);
  sleep(5);

  // Set the minimum priority to the current thread
  if (getuid() == 0)
    pthread_setschedparam(pthread_self(),SCHED_FIFO,&priomin);

  // Set the attributes of each task, including scheduling policy and priority
  for (int i = 0; i < NTASKS; i++) {
    pthread_attr_init(&(attributes[i]));
    pthread_attr_setinheritsched(&(attributes[i]), PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&(attributes[i]), SCHED_FIFO);

    parameters[i].sched_priority = priomin.sched_priority + NTASKS - i;

    pthread_attr_setschedparam(&(attributes[i]), &(parameters[i]));
  }

  int iret[NTASKS];
  struct timeval now;
  struct timezone zone;

  // - Read the current time (gettimeofday)
  // - Set the next arrival time for each task (i.e., the beginning of the next period)
  gettimeofday(&now, &zone);
  for (int i = 0; i < NTASKS; i++) {
      long int periods_micro = periods[i]/1000;
      next_arrival_time[i].tv_sec = now.tv_sec + periods_micro/1000000;
      next_arrival_time[i].tv_usec = now.tv_usec + periods_micro%1000000;
      missed_deadlines[i] = 0;
  }

  // Create all the threads
  iret[0] = pthread_create(&(thread_id[0]), &(attributes[0]), task1, NULL);
  iret[1] = pthread_create(&(thread_id[1]), &(attributes[1]), task2, NULL);
  iret[2] = pthread_create(&(thread_id[2]), &(attributes[2]), task3, NULL);

  for (int i = 0; i < NTASKS; i++) {
    pthread_join(thread_id[i], NULL);
  }

  printf("\n");
  exit(0);
}

// application specific task_1 code

void task1_code() {
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10000; j++) {
		    double uno = rand()*rand();
    }
  }
  printf("1");
  fflush(stdout); // Garantisce che print stampi subito quando è chiamata fflush()
}

void task2_code() {
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10000; j++) {
      double uno = rand()*rand();
	   }
  }
  printf("2");
  fflush(stdout);
}

void task3_code() {
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10000; j++);
    double uno = rand()*rand();
  }
  printf("3");
  fflush(stdout);
}


void *task1(void *ptr) {
  //int i=0;
  struct timespec waittime;
  waittime.tv_sec = 0; /* seconds */
  waittime.tv_nsec = periods[0]; /* nanoseconds */

  // Codice per architetture multicore - altrimenti i thread verrebbero
  // eseguiti su core separati vanificando l'esercizio
  cpu_set_t cset;
  CPU_ZERO( &cset );
  CPU_SET( 0, &cset);
  pthread_setaffinity_np( pthread_self(), sizeof(cpu_set_t), &cset);

  for (int i = 0; i < 100; i++) {
    task1_code();

    struct timeval now;
    struct timezone zone;
    gettimeofday(&now, &zone);

    // After execution, compute the time to the beginning of the next period
    // calcolo il tempo che manca al prossimo periodo
    long int timetowait = 1000*((next_arrival_time[0].tv_sec - now.tv_sec)*1000000
       + (next_arrival_time[0].tv_usec-now.tv_usec));

    if (timetowait < 0) missed_deadlines[0]++;

    waittime.tv_sec = timetowait/1000000000;
    waittime.tv_nsec = timetowait%1000000000;

    nanosleep(&waittime, NULL);

    // the task is ready: set the next arrival time for each task
    // (i.e., the beginning of the next period)

    long int periods_micro = periods[0]/1000;
    next_arrival_time[0].tv_sec = next_arrival_time[0].tv_sec + periods_micro/1000000;
    next_arrival_time[0].tv_usec = next_arrival_time[0].tv_usec + periods_micro%1000000;
  }
}

void *task2(void *ptr) {
  struct timespec waittime;
  waittime.tv_sec = 0; /* seconds */
  waittime.tv_nsec = periods[1]; /* nanoseconds */

  // Codice per architetture multicore - altrimenti i thread verrebbero
  // eseguiti su core separati vanificando l'esercizio
  cpu_set_t cset;
  CPU_ZERO( &cset );
  CPU_SET( 0, &cset);
  pthread_setaffinity_np( pthread_self(), sizeof(cpu_set_t), &cset);

  for (int i = 0; i < 100; i++) {
    task2_code();

    struct timeval now;
    struct timezone zone;
    gettimeofday(&now, &zone);

    // After execution, compute the time to the beginning of the next period
    // calcolo il tempo che manca al prossimo periodo
    long int timetowait = 1000*((next_arrival_time[1].tv_sec - now.tv_sec)*1000000
       + (next_arrival_time[1].tv_usec-now.tv_usec));

    if (timetowait < 0) missed_deadlines[1]++;

    waittime.tv_sec = timetowait/1000000000;
    waittime.tv_nsec = timetowait%1000000000;

    nanosleep(&waittime, NULL);

    // the task is ready: set the next arrival time for each task
    // (i.e., the beginning of the next period)
    long int periods_micro = periods[1]/1000;
    next_arrival_time[1].tv_sec = next_arrival_time[1].tv_sec + periods_micro/1000000;
    next_arrival_time[1].tv_usec = next_arrival_time[1].tv_usec + periods_micro%1000000;
  }
}

void *task3(void *ptr) {
  struct timespec waittime;
  waittime.tv_sec = 0; /* seconds */
  waittime.tv_nsec = periods[2]; /* nanoseconds */

  // Codice per architetture multicore - altrimenti i thread verrebbero
  // eseguiti su core separati vanificando l'esercizio
  cpu_set_t cset;
  CPU_ZERO( &cset );
  CPU_SET( 0, &cset);
  pthread_setaffinity_np( pthread_self(), sizeof(cpu_set_t), &cset);

  for (int i = 0; i < 100; i++) {
    task3_code();

    struct timeval now;
    struct timezone zone;
    gettimeofday(&now, &zone);

    // After execution, compute the time to the beginning of the next period
    // calcolo il tempo che manca al prossimo periodo
    long int timetowait = 1000*((next_arrival_time[2].tv_sec - now.tv_sec)*1000000
       + (next_arrival_time[2].tv_usec-now.tv_usec));

    if (timetowait < 0) missed_deadlines[2]++;

    waittime.tv_sec = timetowait/1000000000;
    waittime.tv_nsec = timetowait%1000000000;

    nanosleep(&waittime, NULL);

    // the task is ready: set the next arrival time for each task
    // (i.e., the beginning of the next period)
    long int periods_micro = periods[2]/1000;
    next_arrival_time[2].tv_sec = next_arrival_time[1].tv_sec + periods_micro/1000000;
    next_arrival_time[2].tv_usec = next_arrival_time[1].tv_usec + periods_micro%1000000;
  }
}
