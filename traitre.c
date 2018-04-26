#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include "reversi.h"
#include "traitre.h"

#define signal 4

//incremente l'age des pions, declenche les trahisons puis en gere les repercussions
//renvoie 0 s'il n'y a pas eu de trahison
int trahison(cellule **plateau, fleche *rose, int **age, int tour, int *N)
{
  int somme=0,i,j,currentage;
  cellule couleur;

  for(i=0;i<*N;i++){
    for(j=0;j<*N;j++){
      if(age[i][j]>0){
        age[i][j]++;
        somme+=age[i][j];
      }
    }
  }
  if(tour>=signal){
    srand(time(NULL));
    for(i=0;i<*N;i++){
      for(j=0;j<*N;j++){
        currentage=age[i][j];
        if(currentage>0){
          if((((float)rand()/INT_MAX)*somme) <= ((float)currentage)){
            couleur=plateau[i][j];
            //XXXXXXXXXXXXXXXXXXXXXXXXXXXXX
            //XXXCOULEURS NON GENERIQUESXXX
            //XXXXXXXXXXXXXXXXXXXXXXXXXXXXX
            if(couleur==vert){
              plateau[i][j]=couleur=rouge;
            }else{
              plateau[i][j]=couleur=vert;
            }
            age[i][j]=1;
            for(int k=0;k<8;k++){
              rose[k].nbcases=checkcapture(plateau,i,j,rose[k].dir,couleur,N);
            }
            capture(plateau,rose,i,j,age,couleur);
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
