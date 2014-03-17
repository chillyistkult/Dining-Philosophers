/*
==========================================================================================
Name : dphil_deadlock.c
Author : Lukas Gruenewald (536059), Benjamin Schuch (533347)
Version : 1.0
Copyright : 2014
Description : Die Philosophen in C. Implementierung mit Deadlock
==========================================================================================
*/


#include <stdio.h>
#include <pthread.h>
#include "dphil.h"



//Repräsentiert die Gabeln
typedef struct sticks {
  pthread_mutex_t **lock; //Ist die Gabel gelockt oder nicht?
  int philCount; //Anzahl der Philosophen
} Sticks;


void pickup(Phil_struct *ps)
{
  Sticks *pp;
  int philCount;

  pp = (Sticks *) ps->v;
  philCount = pp->philCount;

  pthread_mutex_lock(pp->lock[ps->id]);       //Linke Gabel wird gelockt
  sleep(3); //3 Second Delay verursacht Deadlock weil alle Philosophen dann versuchen gleichzeitig pickup aufzurufen
  pthread_mutex_lock(pp->lock[(ps->id+1)%philCount]); // Rechte Gabel wird gelockt
}

void putdown(Phil_struct *ps)
{
  Sticks *pp;
  int i;
  int philCount;

  pp = (Sticks *) ps->v;
  philCount = pp->philCount;

  pthread_mutex_unlock(pp->lock[(ps->id+1)%philCount]); // Rechte Gabel freigeben
  pthread_mutex_unlock(pp->lock[ps->id]);  // Linke Gabel freigeben
}

//Initialisiert Philosophen
void *initialize(int philCount)
{
  Sticks *pp; //Gabeln
  int i;

  pp = (Sticks *) malloc(sizeof(Sticks)); //Speicher reservieren
  pp->philCount = philCount;
  pp->lock = (pthread_mutex_t **) malloc(sizeof(pthread_mutex_t *)*philCount);
  for (i = 0; i < philCount; i++) {
    pp->lock[i] = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  }
  for (i = 0; i < philCount; i++) {
    pthread_mutex_init(pp->lock[i], NULL); //Initialisiert den Mutex ersteinmal mit NULL
  }

  return (void *) pp;
}
