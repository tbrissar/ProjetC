#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "reversi.h"
#include "initreversi.h"
#include "multi.h"
#include "myassert.h"
#include "macros.h"

void jeu(int N, int nbjoueurs, joueur *tabjoueurs, fleche *rose, cellule **plateau)
{
  //printf("JEU\n");
  int cpt = 0, tour = 0, res = 0;

  while(cpt != -1){
    tour++;
    display(plateau, tour, N, nbjoueurs, tabjoueurs);
    res = pose(plateau, rose, tabjoueurs[cpt], tour, N, nbjoueurs, tabjoueurs);
    cpt = (cpt + 1) % nbjoueurs;
    if(res == 0) cpt = checkfin(plateau, rose, tabjoueurs, cpt, N, nbjoueurs);
  }
  scores(plateau,tabjoueurs,N,nbjoueurs);
}

cellule **plateau = NULL;
fleche *rose = NULL;
int N = 0, nbjoueurs = -1;
joueur *tabjoueurs = NULL;

int *tabsock;

int main(int argc, char *argv[])
{
  if(argc < 2){
    error("ERREUR Server:Pas assez d'arguments");
  }

  init(&nbjoueurs, &tabjoueurs, &rose, &N, &plateau);
  tabsock = connectionserver(tabjoueurs, nbjoueurs, argv[1]);

  sendToAll(instrclear,nbjoueurs,tabjoueurs);
  sendToAll(instrtext,nbjoueurs,tabjoueurs);
  sendToAll("La partie commence\n",nbjoueurs,tabjoueurs);
  system(sleepslow);

  jeu(N,nbjoueurs,tabjoueurs,rose,plateau);

  //fermeture des sockets cote serveur
  for(int i = 0; i < nbjoueurs + 1; i++){
    int resClose = close(tabsock[i]);
    myassert(resClose != -1);
    //attente de l'arret des fils si l'on est en local
    if(strcmp(argv[1], "local") == 0) wait(NULL);
  }

  terminate(plateau, rose, N, tabjoueurs, tabsock);

  return 0;
}
