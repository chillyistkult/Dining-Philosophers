/*
==========================================================================================
Name : dphil_final.c
Author : Lukas Gruenewald (536059), Benjamin Schuch (533347)
Version : 1.0
Copyright : 2014
Description : Die Philosophen in C. Implementierung ohne Deadlock
==========================================================================================
*/

#include <stdio.h>
#include <pthread.h>
#include "dphil.h"


#define THINKING 0
#define HUNGRY 1
#define EATING 2

//Struct repräsentiert einen einzelnen Philosophen
typedef struct {
  pthread_mutex_t *mon; //Monitor für den Philosophen
  pthread_cond_t **cv;
  int *state; //Aktueller State (EAT, THINKING, SLEEPING)
  int *queuenum; //Nummer in der Warteschlange
  int qn; //Zwischenspeicher für die Warteschlange
  int phil_count; //Anzahl der vorhanden Philosophen
} Phil;

//Methode bestimmt ob ein Philosoph essen kann oder nicht
int can_I_eat(Phil *pp, int n)
{
  int phil_count;

  phil_count = pp->phil_count;

  //Wenn
  if (pp->state[(n+(phil_count-1))%phil_count] == EATING ||
      pp->state[(n+1)%phil_count] == EATING) return 0;

  if (pp->state[(n+(phil_count-1))%phil_count] == HUNGRY &&
      pp->queuenum[(n+(phil_count-1))%phil_count] < pp->queuenum[n]) return 0;

  if (pp->state[(n+1)%phil_count] == HUNGRY &&
      pp->queuenum[(n+1)%phil_count] < pp->queuenum[n]) return 0;

  return 1;
}

static char stc[3] = {'T', 'H', 'E'}; //Thinking, Hungry, Eating

void pickup(Phil_struct *ps)
{
  Phil *pp;

  pp = (Phil *) ps->v;


  pthread_mutex_lock(pp->mon);
  pp->state[ps->id] = HUNGRY;	//Status wird ersteinmal auf hungrig gesetzt...
  pp->queuenum[ps->id] = pp->qn++;	//.. und der Philosoph in die Warteschlange eingereiht
  while (!can_I_eat(pp, ps->id)) {	//Solange der Philosoph noch nicht essen kann wartet der Thread
    pthread_cond_wait(pp->cv[ps->id], pp->mon);
  }
  pp->state[ps->id] = EATING; //Dann wird der Status auf essen gesetzt
  pthread_mutex_unlock(pp->mon);
}

void putdown(Phil_struct *ps)
{
  Phil *pp;
  int phil_count;

  pp = (Phil *) ps->v;
  phil_count = pp->phil_count;

  pthread_mutex_lock(pp->mon);
  pp->state[ps->id] = THINKING; //Status wird auf denken gesetzt
  //Threads der anderen Philosophen werden benachrichtigt
  pthread_cond_signal(pp->cv[(ps->id+(phil_count-1))%phil_count]);
  pthread_cond_signal(pp->cv[(ps->id+1)%phil_count]);
  pthread_mutex_unlock(pp->mon);
}

//Initialisiert Philosophen
void *initialize(int phil_count)
{
  Phil *pp;
  int i;


  pp = (Phil *) malloc(sizeof(Phil));
  pp->phil_count = phil_count;
  pp->mon = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pp->cv = (pthread_cond_t **) malloc(sizeof(pthread_cond_t *)*phil_count);
  //Speicherreservierung
  if (pp->cv == NULL) { perror("malloc"); exit(1); }
  pp->state = (int *) malloc(sizeof(int)*phil_count);
  if (pp->state == NULL) { perror("malloc"); exit(1); }
  pp->queuenum = (int *) malloc(sizeof(int)*phil_count);
  if (pp->queuenum == NULL) { perror("malloc"); exit(1); }
  pp->qn = 0; //Die Nummer in der Warteschleife ist ersteinmal 0
  pthread_mutex_init(pp->mon, NULL);
  for (i = 0; i < phil_count; i++) {
    pp->cv[i] = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
    pthread_cond_init(pp->cv[i], NULL);
    pp->state[i] = THINKING; //Alle Philosophen starten ersteinmal mit denken
  }

  return (void *) pp;
}
