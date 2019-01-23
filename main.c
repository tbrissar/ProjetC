#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "macros.h"

  char heberge[1],jouer[2],modejeu[1];

int main()
{
  system(clear);
  do{
    printf("Voulez-vous jouer en local ou en reseau ? [l/r]\n");
    scanf("%s",modejeu);
  }while(strcmp(modejeu,"l") != 0 && strcmp(modejeu,"r") != 0);

///////////////////////////////////////

  if(strcmp(modejeu,"l") == 0){

  //PARTIE EN LOCAL

    system("./server local");

/////////////////////////////////////

  }else if(strcmp(modejeu,"r") == 0){

  //PARTIE EN RESEAU

    //on demande au joueur s'il souhaite héberger la partie
    while(strcmp(heberge,"y") != 0 && strcmp(heberge,"n") != 0){
      printf("Souhaitez vous héberger la partie ? [y/n]\n");
      scanf("%s",heberge);
    }

    //si le joueur héberge on lance un serveur
    if(strcmp(heberge,"y") == 0 && !fork()){
      system("gnome-terminal --command=\"./server serveur\"");
    }

    //on demande ensuite s'il veut jouer
    if(strcmp(heberge,"n") == 0){
      while(strcmp(jouer,"y") != 0 && strcmp(jouer,"n") != 0){
        printf("Souhaitez vous jouer ? [y/n]\n");
        scanf("%s",jouer);
      }
    }else{
      jouer[1] = 'y';
    }

    //si le joueur veut jouer on lance un client
    if(strcmp(jouer,"y") == 0 && !fork()){
      printf("je lance un client\n");
      system("gnome-terminal --command=\"./client NULL\"");
      exit(0);
    }

    if(strcmp(heberge,"y") == 0) wait(NULL);
    if(strcmp(jouer,"y") == 0) wait(NULL);
  }
  return (0);
}
