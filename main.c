#include <stdio.h>
#include <stdlib.h>
#include "reversi.h"
#include "initreversi.h"

  cellule **plateau=NULL;
  fleche *rose=NULL;
  int cpt=0,res,tour=0,N=0,nbjoueurs=0;
  joueur *tabjoueurs;

int main()
{
  rose = initrose();

  tabjoueurs=initJoueurs(&nbjoueurs);
  plateau=initplateau(&N,nbjoueurs,tabjoueurs,rose);
  //ne rien changer avant

  while(cpt!=-1){
    tour++;
    affichage(plateau,tour,N);
    res=pose(plateau,rose,tabjoueurs[cpt],tour,N,nbjoueurs);
    cpt=(cpt+1)%nbjoueurs;
    if(res==0){
      cpt=checkfin(plateau,rose,tabjoueurs,cpt,N,nbjoueurs);
    }
  }
  scores(plateau,tabjoueurs,N,nbjoueurs);

  //ne rien changer apres
  terminate(plateau,rose,N);
  return (0);
}
