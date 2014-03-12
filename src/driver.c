/*
==========================================================================================
Name : driver.c
Author : Lukas Gruenewald (536059), Benjamin Schuch (533347)
Version : 1.0
Copyright : 2014
Description : Die Philosophen in C. Treiber zur Steuerung der beiden Implementierungen
==========================================================================================
*/

#include <stdio.h>
#include <pthread.h>
#include "dphil.h"


/** Die Funktion bildet die Kernlogik der Philosophen ab und steuert seinen
	seinen "Alltag", d.h. wann er isst, denkt und schläft. Außerdem gibt er die Blockzeiten
	für den Beobachter frei
    @param void *v - Zeigt auf die void Methode, mit der die Philosophen initialisiert werden.
*/
void *philosopher(void *v)
{
  Phil_struct *ps;
  long st;
  long t;

  ps = (Phil_struct *) v;

  while(1) {

    // Als erstes fängt der Philosoph an für eine zufällige Zeit zu denken

    st = (random()%ps->ms) + 1;
    printf("%3d Philosoph %d denkt für %d Sekunde%s\n",
                time(0)-ps->t0, ps->id, st, (st == 1) ? "" : "n");
    fflush(stdout);
    sleep(st);

    // Nachdem denken fängt der Philosoph an zu essen und ruft die Methode pickup
    // auf um eine Gabel zu nehmen.

    printf("%3d Philosoph %d denkt nicht mehr -- ruft pickup() auf\n",
            time(0)-ps->t0, ps->id);
    fflush(stdout);
    t = time(0);
    pthread_mutex_lock(ps->blockmon);
    ps->blockstarting[ps->id] = t;
    pthread_mutex_unlock(ps->blockmon);

    pickup(ps);

    pthread_mutex_lock(ps->blockmon);
    ps->blocktime[ps->id] += (time(0) - t);
    ps->blockstarting[ps->id] = -1;
    pthread_mutex_unlock(ps->blockmon);

    // Wenn die Methode pickup durchlaufen werden konnte isst der Philosoph für
    // eine zufällig bestimmte Zeit

    st = (random()%ps->ms) + 1;
    printf("%3d Philosoph %d isst für %d Sekunde%s\n",
                time(0)-ps->t0, ps->id, st, (st == 1) ? "" : "n");
    fflush(stdout);
    sleep(st);

    // Wenn der Philosoph aufgegessen hat ruft er die Methode putdown auf um die
    // Gabel wieder freizugeben

    printf("%3d Philosoph %d isst nicht mehr -- ruft putdown() auf\n",
            time(0)-ps->t0, ps->id);
    fflush(stdout);
    putdown(ps);
  }
}


/** Die Funktion main wird als erstes, direkt nach Programmstart
    aufgerufen. Das Programm muss mit 3 Kommandozeilenparametern
    aufgerufen werden. Es werden je nach übergebener Anzahl an Philosophen
    entsprechend viele Threads gestartet und initiale Werte gesetzt. Die
    while-Schleife kümmert sich dann um die Ausgabe auf die Konsole, in dem
    die entsprechenden Werte aus dem Philosophen Struct ausgelesen, verarbeitet
    und aufbereitet werden.
    @param argc - Anzahl der Kommandozeilenparameter
    @param argv - Feld von Strings, das die Kommandozeilenparameter enthaelt
*/
main(argc, argv)
int argc;
char **argv;
{
  int i;
  struct tm *tm;
  pthread_t *threads;
  Phil_struct *ps;
  void *v;
  long t0, ttmp, ttmp2;
  pthread_mutex_t *blockmon; //Der Monitor der für das aufzeichnen der Blockzeiten zuständig ist
  int *blocktime;	//Die geblockte Zeit in Sekunden
  int *blockstarting;	//Der Zeitpunkt des ersten Blocks
  char s[500];
  int philCount;	//Anzahl der Philosophen
  char *curr;
  int total;	//Gesamte Blockzeit (Summe der Blockzeiten aller Philosophen)

  if (argc != 3) {
    fprintf(stderr, "usage: Parameter = :: Anzahl der Philosophen :: Maximale Zeit, die ein Philosoph isst, denkt bzw. schläft\n");
    exit(1);
  }

  srandom(time(0)); //Seed des random generators

  philCount = atoi(argv[1]);
  //Speicher wird allokiert
  threads = (pthread_t *) malloc(sizeof(pthread_t)*philCount);
  ps = (Phil_struct *) malloc(sizeof(Phil_struct)*philCount);



  v = initialize(philCount);
  t0 = time(0);	//Startzeit
  //Speicher wird allokiert
  blocktime = (int *) malloc(sizeof(int)*philCount);
  blockstarting = (int *) malloc(sizeof(int)*philCount);

  blockmon = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(blockmon, NULL); //Initialisiert den mutex für den Blockmonitor

  //Setzt die Blockzeiten und die Startzeit für alle Philosophen auf 0 bzw. -1
  for (i = 0; i < philCount; i++) {
    blocktime[i] = 0;
    blockstarting[i] = -1;
  }

  //Philosophenstructs werden mit den entsprechenden werden initialisiert
  for (i = 0; i < philCount; i++) {
    ps[i].id = i;
    ps[i].t0 = t0;
    ps[i].v = v;
    ps[i].ms = atoi(argv[2]);
    ps[i].blocktime = blocktime;
    ps[i].blockstarting = blockstarting;
    ps[i].blockmon = blockmon;
    ps[i].philCount = philCount;
    pthread_create(threads+i, NULL, philosopher, (void *) (ps+i)); //Threads entsprechend der Anzahl an Philosophen werden angelegt
  }

  //Schleife für den Beobachter (Konsolenoutput)
  while(1) {
    pthread_mutex_lock(blockmon);
    ttmp = time(0);
    curr = s;
    total = 0;
    //Gesamte Blockzeit wird berechnet
    for(i=0; i < philCount; i++) {
      total += blocktime[i];
      if (blockstarting[i] != -1) total += (ttmp - blockstarting[i]);
    }
    sprintf(curr,"%3d Geblockte Zeit: %5d : ", ttmp-t0, total);

    curr = s + strlen(s);

    for(i=0; i < philCount; i++) {
      ttmp2 = blocktime[i];
      if (blockstarting[i] != -1) ttmp2 += (ttmp - blockstarting[i]);
      sprintf(curr, "%5d ", ttmp2);
      curr = s + strlen(s);
    }
    pthread_mutex_unlock(blockmon);
    printf("%s\n", s);
    fflush(stdout);
    sleep(10);

    //Programm (Beobachter) bricht nach spätestens 120 Sekunden ab...
    if(difftime( time(0), t0) > 120) {
    	printf("boring...");
		exit(1);
    }
  }
}
