/*
==========================================================================================
Name : driver.c
Author : Lukas Gruenewald (536059), Benjamin Schuch (533347)
Version : 1.0
Copyright : 2014
Description : Die Philosophen in C. Header-File
==========================================================================================
*/

#include <stdlib.h>
#include <string.h>

typedef struct {
  int id;                // Fortlaufende ID der Philosophen
  long t0;               // Startzeit
  long ms;               // Die maximale Zeit, die die Philosophen essen, schlafen...
  void *v;               // Welche Methode soll zum initialisieren aufgerufen werden?
  int *blocktime;        // Die Zeit die ein Philosoph geblockt ist
  int *blockstarting;    // Die Startzeit nachdem ein Philosoph geblockt wurde
  int philCount;

  pthread_mutex_t *blockmon;   // Monitor für die geblockten Zeiten
} Phil_struct;

extern void *initialize(int philCount);
extern void pickup(Phil_struct *);
extern void putdown(Phil_struct *);
