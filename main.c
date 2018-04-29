#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "macros.h"

  char heberge='a',jouer='a',modejeu[1];

int main()
{
  system(clear);
  do{
    printf("Voulez-vous jouer en local ou en reseau ? [l/r]\n");
    scanf("%s",modejeu);
  }while(strcmp(modejeu,"l")!=0 && strcmp(modejeu,"r")!=0);

///////////////////////////////////////

  if(strcmp(modejeu,"l")==0){

  //PARTIE EN LOCAL

    system("./server local");

/////////////////////////////////////

}else if(strcmp(modejeu,"r")==0){

  //PARTIE EN RESEAU

    //on demande au joueur s'il souhaite héberger la partie
    while(heberge!='y' && heberge!='n'){
      printf("Souhaitez vous héberger la partie ? [y/n]\n");
      scanf("%c",&heberge);
    }

    //si le joueur héberge on lance un serveur
    if(heberge=='y' && !fork()){
      system("gnome-terminal --command=\"./server serveur\"");
    }

    //on demande ensuite s'il veut jouer
    if(heberge=='y'){
      while(jouer!='y' && jouer!='n'){
        printf("Souhaitez vous jouer ? [y/n]\n");
        scanf("%c",&jouer);
      }
    }else{
      jouer='y';
    }

    //si le joueur veut jouer on lance un client
    if(jouer=='y' && !fork()){
      system("gnome-terminal --command=\"./client\"");
      exit(0);
    }

    if(heberge=='y'){
      wait(NULL);
    }
    if(jouer=='y'){
      wait(NULL);
    }
  }
  return (0);
}
