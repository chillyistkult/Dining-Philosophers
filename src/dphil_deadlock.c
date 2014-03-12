/*
==========================================================================================
Name : driver.c
Author : Lukas Gruenewald (536059), Benjamin Schuch (533347)
Version : 1.0
Copyright : 2014
Description : Die Philosophen in C. Implementierung mit Deadlock
==========================================================================================
*/


/*
#include <stdio.h>
#include <pthread.h>
#include "dphil.h"




typedef struct sticks {
  pthread_mutex_t **lock;
  int philCount;
} Sticks;


void pickup(Phil_struct *ps)
{
  Sticks *pp;
  int philCount;

  pp = (Sticks *) ps->v;
  philCount = pp->philCount;

  pthread_mutex_lock(pp->lock[ps->id]);       // lock up left stick
  sleep(3);
  pthread_mutex_lock(pp->lock[(ps->id+1)%philCount]); // lock up right stick
}

void putdown(Phil_struct *ps)
{
  Sticks *pp;
  int i;
  int philCount;

  pp = (Sticks *) ps->v;
  philCount = pp->philCount;

  pthread_mutex_unlock(pp->lock[(ps->id+1)%philCount]); // unlock right stick
  pthread_mutex_unlock(pp->lock[ps->id]);  // unlock left stick
}

void *initialize(int philCount)
{
  Sticks *pp;
  int i;

  pp = (Sticks *) malloc(sizeof(Sticks));
  pp->philCount = philCount;
  pp->lock = (pthread_mutex_t **) malloc(sizeof(pthread_mutex_t *)*philCount);
  if (pp->lock == NULL) { perror("malloc"); exit(1); }
  for (i = 0; i < philCount; i++) {
    pp->lock[i] = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
    if (pp->lock[i] == NULL) { perror("malloc"); exit(1); }
  }
  for (i = 0; i < philCount; i++) {
    pthread_mutex_init(pp->lock[i], NULL);
  }

  return (void *) pp;
}
*/
