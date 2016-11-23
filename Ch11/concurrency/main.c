/* Concurrency: main.c
 *
 * Copyright (c) 2016 by Giulio Zambon.  All rights reserved.
 *
 * You are free to use (without any warranty), redistribute, and/or modify
 * this software under the terms of the GNU General Public License version 2
 * (http://gnu.org/licenses/gpl-2.0.html).
 */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

#define Crash(...) {                                            \
  fprintf(stderr, "\n");                                        \
  fprintf(stderr, __VA_ARGS__);                                 \
  fprintf(stderr, " (file %s; line %d)\n", __FILE__, __LINE__); \
  return EXIT_FAILURE;                                          \
  }

int an_int = 0;
int thread_running = 0;
pthread_mutex_t a_mutex = PTHREAD_MUTEX_INITIALIZER;

void *a_thread(void *x);

//------------------------------------------------------------------------ main
int main(int argc, char *argv[]) {

  // Create a thread.
  pthread_t thread;
  int res = pthread_create(&thread, NULL, &a_thread, NULL);
  if (res) Crash("Main: error %d when creating thread.", res);            //-->
  printf("Main: created thread\n");

  // Access global variable concurrently with the thread.
  while (!thread_running) sleep(1);
  while (thread_running) {
    pthread_mutex_lock(&a_mutex);
    an_int = 0;
    pthread_mutex_unlock(&a_mutex);
    }

  // Display the counter returned by the thread.
  long ret;
  res = pthread_join(thread, (void *)&ret);
  if (res) Crash("Main: thread ended with error %d", res);                //-->
  printf("Main: thread has ended after %ld iterations.\n", ret);

	return EXIT_SUCCESS;
  }

//-------------------------------------------------------------------- a_thread
const static struct timespec m20 = {0, 20000000L};
void *a_thread(void *x) {
  printf("Thread: I'm running\n");
  thread_running = 1;
  unsigned long kount = 0;
  int match;
  do {
    kount++;
    pthread_mutex_lock(&a_mutex);
    an_int = kount;
    nanosleep(&m20, NULL);
    match = (an_int == kount);
    pthread_mutex_unlock(&a_mutex);
    } while (match && kount < 500);
  thread_running = 0;
  return (void *)kount;
  } // a_thread
