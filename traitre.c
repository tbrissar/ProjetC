#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include "reversi.h"
#include "traitre.h"

#define signal 4

//incremente l'age des pions, declenche les trahisons puis en gere les repercussions
//renvoie 0 s'il n'y a pas eu de trahison
int trahison(cellule **plateau, fleche *rose, int tour, int N)
{
  int somme=0,i,j,currentage;
  color coul,macoul;

  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
      if(plateau[i][j].age>0){
        plateau[i][j].age++;
        somme+=plateau[i][j].age;
      }
    }
  }
  if(tour>=signal){
    srand(time(NULL));
    for(i=0;i<N;i++){
      for(j=0;j<N;j++){
        currentage=plateau[i][j].age;
        if(currentage>0){
          if((((float)rand()/INT_MAX)*somme) <= ((float)currentage)){
            macoul=coul=plateau[i][j].couleur;
            while(macoul==coul){
              macoul=couleuraleatoire();
            }
            plateau[i][j].couleur=macoul;
            plateau[i][j].age=1;
            for(int k=0;k<8;k++){
              rose[k].nbcases=checkcapture(plateau,i,j,rose[k].dir,macoul,N);
            }
            capture(plateau,rose,i,j,macoul);
            printf("%d,%d a trahi! Le fourbe!\n",i,j);
            return(1);
          }
          somme-=currentage;
        }
      }
    }
  }
  return(0);
}
