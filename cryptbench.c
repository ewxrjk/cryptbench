#define _GNU_SOURCE 1
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <sys/sysinfo.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <crypt.h>

// Command-line options
const struct option options[] = {
    {
        "salt",
        required_argument,
        NULL,
        's',
    },
    {
        "threads",
        required_argument,
        NULL,
        'j',
    },
    {
        "iterations",
        required_argument,
        NULL,
        'i',
    },
    {
        "help",
        no_argument,
        NULL,
        'h',
    },
    {NULL, 0, NULL, 0},
};

struct config {
  long nhashes;
  int nthreads;
  const char *salt;
};

static void *worker(void *arg) {
  const struct config *c = arg;
  struct crypt_data cd;

  for(long i = 0; i < c->nhashes; i++) {
    cd.initialized = 0;
    if(!crypt_r("password", c->salt, &cd)) {
      perror("crypt_r");
      exit(1);
    }
  }
  return NULL;
}

int main(int argc, char **argv) {
  int n;
  struct timeval starttv, finishtv;
  struct config c;

  c.nthreads = 0;
  c.nhashes = 10000;
  c.salt = "xx";
  while((n = getopt_long(argc, argv, "+hH:j:i:", options, NULL)) >= 0) {
    switch(n) {
    case 'h':
      printf("Usage:\n"
             "  cryptbench [OPTIONS]\n"
             "Options:\n"
             "  --salt,-s SALT      Salt string\n"
             "  --threads, -j N     Number of threads to use\n"
             "  --iterations, -i N  Number of iterations per thread\n");
      exit(0);
    case 's': c.salt = optarg; break;
    case 'j': c.nthreads = atoi(optarg); break;
    case 'i': c.nhashes = atol(optarg); break;
    default: exit(1);
    }
  }
  if(c.nthreads == 0)
    c.nthreads = get_nprocs();
  pthread_t threads[c.nthreads];
  if(gettimeofday(&starttv, NULL) < 0) {
    perror("gettimeofday");
    exit(1);
  }
  for(int i = 0; i < c.nthreads; i++) {
    int e;
    if((e = pthread_create(&threads[i], NULL, worker, &c)) != 0) {
      fprintf(stderr, "pthread_create: %s\n", strerror(e));
      exit(1);
    }
  }
  for(int i = 0; i < c.nthreads; i++) {
    int e;
    if((e = pthread_join(threads[i], NULL)) != 0) {
      fprintf(stderr, "pthread_join: %s\n", strerror(e));
      exit(1);
    }
  }
  if(gettimeofday(&finishtv, NULL) < 0) {
    perror("gettimeofday");
    exit(1);
  }
  double start = starttv.tv_sec + starttv.tv_usec / 1000000.0;
  double finish = finishtv.tv_sec + finishtv.tv_usec / 1000000.0;
  double elapsed = finish - start;
  double crypts = c.nthreads * (double)c.nhashes;
  printf("threads %d elapsed %g iterations %g crypts/sec %g\n", c.nthreads,
         elapsed, crypts, crypts / elapsed);
  return 0;
}