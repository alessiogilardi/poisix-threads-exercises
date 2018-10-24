/*
compile with:
	g++ threadex1_orig.cpp -lpthread -o thread1

run with:
	.\thread1
*/


// Create two independent threads each of which will execute the function
// print_message_function, by passing a different string as the fourth parameter.
// That is, the first thread has message1 as the fourth parameter, the second thread has
// message2 as the fourth parameter


// Wait till threads are complete before main continues. Unless we
// wait we run the risk of executing an exit which will terminate
// the process and all threads before the threads have completed.

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *print_message_function( void *ptr );
void *print_message_function_2( void *ptr );

int main()
{
     //declare the ids of thread 1 and thread 2
     pthread_t thread1, thread2;

     // Valore di ritorno dei thread
     void *ret;

     // Parametri per i Thread
     const char *message1 = "Hello from Thread 1";
     const char *message2 = "Hello from Thread 2";

     // Valori di ritorno dai Thread
     int  iret1, iret2;

     iret1 = pthread_create(&thread1, NULL, print_message_function_2, (void *) message1);
     iret2 = pthread_create(&thread2, NULL, print_message_function_2, (void *) message2);

     pthread_join(thread1, &ret);
     pthread_join(thread2, &ret);



     printf("Thread 1 returns: %d\n",ret);
     printf("Thread 2 returns: %d\n",ret);
     exit(0);
}

void *print_message_function(void *ptr)
{
     char *message;
     message = (char *) ptr;
     int i;
     for (i=0; i < 3; i++)
     {
       printf("%s\n", message);
     }

}

void *print_message_function_2(void *ptr)
{
     char *message;
     int *ret;
     message = (char *) ptr;
     int i;
     for (i=0; i < 3; i++)
     {
       printf("%s\n", message);
     }
     ret = i;
     pthread_exit(ret);
}

// Optional exercise: when exiting from the thread,
// use pthread_exit and properly set the error code,
// such that this value can be retrieved in the main thread
// when using the join function

// Optional exercise: try to set the real-time policy RT_FIFO before creating threads.
