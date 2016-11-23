/* Interrupts: main.c
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
#include "numbers.h"
#include "binary.h"
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define N_THREADS 3

#define Crash(...) {                                            \
  fprintf(stderr, "\n");                                        \
  fprintf(stderr, __VA_ARGS__);                                 \
  fprintf(stderr, " (file %s; line %d)\n", __FILE__, __LINE__); \
  return EXIT_FAILURE;                                          \
  }

#define N_DEF_SIGS 32
char *sig_names[N_DEF_SIGS] = {
    /* 0*/ "0        ",
    /* 1*/ "SIGHUP   ", // Hangup (POSIX)
    /* 2*/ "SIGINT   ", // Interrupt (ANSI)
    /* 3*/ "SIGQUIT  ", // Quit (POSIX)
    /* 4*/ "SIGILL   ", // Illegal instruction (ANSI)
    /* 5*/ "SIGTRAP  ", // Trace trap (POSIX)
    /* 6*/ "SIGABRT  ", // Abort (ANSI)
    /* 7*/ "SIGBUS   ", // BUS error (4.2 BSD)
    /* 8*/ "SIGFPE   ", // Floating-point exception (ANSI)
    /* 9*/ "SIGKILL  ", // Kill, unblockable (POSIX)
    /*10*/ "SIGUSR1  ", // User-defined signal 1 (POSIX)
    /*11*/ "SIGSEGV  ", // Segmentation violation (ANSI)
    /*12*/ "SIGUSR2  ", // User-defined signal 2 (POSIX)
    /*13*/ "SIGPIPE  ", // Broken pipe (POSIX)
    /*14*/ "SIGALRM  ", // Alarm clock (POSIX)
    /*15*/ "SIGTERM  ", // Termination (ANSI)
    /*16*/ "SIGSTKFLT", // Stack fault
    /*17*/ "SIGCHLD  ", // Child status has changed (POSIX)
    /*18*/ "SIGCONT  ", // Continue (POSIX)
    /*19*/ "SIGSTOP  ", // Stop, unblockable (POSIX)
    /*20*/ "SIGTSTP  ", // Keyboard stop (POSIX)
    /*21*/ "SIGTTIN  ", // Background read from tty (POSIX)
    /*22*/ "SIGTTOU  ", // Background write to tty (POSIX)
    /*23*/ "SIGURG   ", // Urgent condition on socket (4.2 BSD)
    /*24*/ "SIGXCPU  ", // CPU limit exceeded (4.2 BSD)
    /*25*/ "SIGXFSZ  ", // File size limit exceeded (4.2 BSD)
    /*26*/ "SIGVTALRM", // Virtual alarm clock (4.2 BSD)
    /*27*/ "SIGPROF  ", // Profiling alarm clock (4.2 BSD)
    /*28*/ "SIGWINCH ", // Window size change (4.3 BSD, Sun)
    /*29*/ "SIGIO    ", // I/O now possible (4.2 BSD)
    /*30*/ "SIGPWR   ", // Power failure restart (System V)
    /*31*/ "SIGSYS   "  // Bad system call"
    };

pthread_t threads[N_THREADS] = {};

void *a_thread(void *x);
int identify_thread(pthread_t thr);
void signal_handler(int sig_num);

//------------------------------------------------------------------------ main
int main(int argc, char *argv[]) {
  int res;

  // Catch some signals.
  struct sigaction act = {{&signal_handler}};
  struct sigaction oldact = {};
  if (signal(SIGKILL, signal_handler) == SIG_ERR) {
    printf("Main: can't catch SIGKILL\n");
    }
  //
  if (signal(SIGINT, signal_handler) == SIG_ERR) {
    printf("Main: can't catch SIGINT\n");
    }
  //
  if (sigaction(SIGABRT, &act, &oldact) == -1) {
    printf("Main: can't catch SIGABRT\n");
    }
  printf("%p, %p\n", signal_handler, oldact.sa_handler);
  if (sigaction(SIGABRT, &act, &oldact) == -1) {
    printf("Main: can't catch SIGABRT\n");
    }
  printf("%p, %p\n", signal_handler, oldact.sa_handler);

//  pthread_attr_t my_attr;
//  pthread_attr_init(&my_attr);
//  size_t stack_size;
//  pthread_attr_getstacksize(&my_attr, &stack_size);
//  printf("Default stack size: %zu\n", stack_size);

  // Create three threads.
  for (int k = 0; k < N_THREADS; k++) {
    res = pthread_create(&threads[k], NULL, &a_thread, NULL);
    if (res) Crash("Main: error %d when creating thread %d", res, k);     //-->
    printf("Main: created thread %d (%u)\n", k, (unsigned int)threads[k]);
    }

  // Terminate the threads one by one.
  sleep(2);
  printf("Canceling thread 0\n");
  res = pthread_cancel(threads[0]);
  if (res) {
    Crash("Main: \"%s\" when canceling thread %d", strerror(res), 0);     //-->
    }
  sleep(2);
  printf("Killing thread 1\n");
  res = pthread_kill(threads[1], SIGINT);
  if (res) {
    Crash("Main: \"%s\" when killing thread %d", strerror(res), 1);       //-->
    }
  sleep(2);
  printf("Aborting thread 2\n");
  res = pthread_kill(threads[2], SIGABRT);
  if (res) {
    Crash("Main: \"%s\" when aborting thread %d", strerror(res), 2);      //-->
    }
  sleep(2);
/*
  printf("Aborting thread 0\n");
  res = pthread_kill(threads[0], SIGABRT);
  if (res) {
    Crash("Main: \"%s\" when aborting thread %d", strerror(res), 0);      //-->
    }
*/

  // Wait for the threads to end.
  for (int k = 0; k < N_THREADS; k++) {
    long ret;
    res = pthread_join(threads[k], (void *)&ret);
    if (res) Crash("Main: thread %d ended with error %d", k, res);        //-->
    printf("Main: thread %d has ended with result ", k);
    if (ret > 0 && ret < N_DEF_SIGS) printf("%s\n", sig_names[ret]);
    else printf("%ld\n", ret);
    }

  printf("Main: that's it folks!\n");




/*
 // Displaying bytes in binary format.
  unsigned int i = 0x12345678;
  printf("%p: 0x%08x\n", &i, i);
  char si[9];
  printf("\n    <--Address--->    <--Content----->\n");
  for (int k = 0; k < sizeof(i); k++) {
    unsigned char *cp = ((unsigned char *)&i) + k;
    num_binfmt(cp, 8, si, 0);
    printf("    %p    0x%02x  0b%s\n", cp, *cp, si);
    }

  // Binary operations.
  printf("0x%x\n", bin_to_uint("  0b00xxx  ccc 010010"));
  char *mask_s = "0b00001111";
  char *n1_s = "0b01110010";
  char res_s[9];
  unsigned int res;
  unsigned int mask = bin_to_uint(mask_s);
  unsigned int n1 = bin_to_uint(n1_s);

  res = n1 & mask;
  num_binfmt(&res, 8, res_s, 0);
  printf("%s &  %s = 0b%s\n", n1_s, mask_s, res_s);
  res = n1 & ~mask;
  num_binfmt(&res, 8, res_s, 0);
  printf("%s & ~%s = 0b%s\n", n1_s, mask_s, res_s);

  res = n1 | mask;
  num_binfmt(&res, 8, res_s, 0);
  printf("%s |  %s = 0b%s\n", n1_s, mask_s, res_s);
  res = n1 | ~mask;
  num_binfmt(&res, 8, res_s, 0);
  printf("%s | ~%s = 0b%s\n", n1_s, mask_s, res_s);

  res = n1 ^ mask;
  num_binfmt(&res, 8, res_s, 0);
  printf("%s ^  %s = 0b%s\n", n1_s, mask_s, res_s);
  res = n1 ^ ~mask;
  num_binfmt(&res, 8, res_s, 0);
  printf("%s ^ ~%s = 0b%s\n", n1_s, mask_s, res_s);

  unsigned int n2 = 0xabcdef76 >> 4;
  char n2_s[36];
  num_binfmt(&n2, 32, n2_s, 1);
  printf("\nn2=%s\n", n2_s);
  }
*/
	return EXIT_SUCCESS;
  } // main

//-------------------------------------------------------------------- a_thread
void *a_thread(void *x) {
  pthread_t t = pthread_self();
  printf("Thread %d (%u): I'm running\n", identify_thread(t), (unsigned int)t);
  while (1) sleep(10);
  return NULL;
  } // a_thread

//------------------------------------------------------------- identify_thread
int identify_thread(pthread_t thr) {
  for (int k = 0; k < N_THREADS; k++) if (thr == threads[k]) return k;    //-->
  return '?' - '0';
  } // identify_thread

//-------------------------------------------------------------- signal_handler
void signal_handler(int sig_num) {
  char thread_num_s[2] = {};
  thread_num_s[0] = '0' + identify_thread(pthread_self());
  write(1, "Thread ", 7);
  write(1, thread_num_s, 1);
  write(1, ": received signal ", 18);
  if (sig_num < N_DEF_SIGS) {
    write(1, sig_names[sig_num], 9);
    }
  else {
    char s[3] = {' ', ' ', '\0'};
    s[0] = '0' + sig_num / 10;
    s[1] = '0' + sig_num % 10;
    write(1, s, 2);
    }
  write(1, "\n", 1);
  pthread_exit((void *)(long)sig_num);
  } // signal_handler
