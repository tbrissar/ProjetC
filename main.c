#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> //to use hostent
#include "reversi.h"
#include "initreversi.h"
#include "multi.h"

#ifdef WIN32
#define clear "cls"
#define sleep "CHOICE /T 1"
#else
#define clear "clear"
#define sleep "sleep 1"
#endif

#define taillebuffer 3000

  void jeu(int N, int nbjoueurs, joueur *tabjoueurs, fleche *rose, cellule **plateau, int modejeu)
  {
    int cpt=0,tour=0,res;

    while(cpt!=-1){
      tour++;
      affichage(plateau,tour,N,modejeu,nbjoueurs,tabjoueurs);
      res=pose(plateau,rose,tabjoueurs[cpt],tour,N,nbjoueurs,modejeu,tabjoueurs);
      cpt=(cpt+1)%nbjoueurs;
      if(res==0){
        cpt=checkfin(plateau,rose,tabjoueurs,cpt,N,nbjoueurs);
      }
    }
    scores(plateau,tabjoueurs,N,nbjoueurs);
  }

  cellule **plateau=NULL;
  fleche *rose=NULL;
  int N=0,nbjoueurs=0,multirole=-1,n;
  // n: return value for write() and read()
  joueur *tabjoueurs;

  int sockfd,modejeu;

int main()
{
  system(clear);
  printf("Local:0 Multi:1\n");
  scanf("%d",&modejeu);

  rose = initrose();

  tabjoueurs=initJoueurs(&nbjoueurs);
  plateau=initplateau(&N,nbjoueurs,tabjoueurs,rose);
  connectionserver(tabjoueurs,nbjoueurs);
  jeu(N,nbjoueurs,tabjoueurs,rose,plateau,modejeu);


///////////////////////////////////////

if(modejeu==0){

  //PARTIE EN LOCAL





/////////////////////////////////////


  //PARTIE EN RESEAU

  }else if(modejeu==1){
    while(multirole!=1 && multirole !=0){
      printf("Hote:1 Client:0\n");
      scanf("%d",&multirole);
    }

    //client
    }else if(multirole==0){
      sockfd=connectionclient();
      char *buffer=malloc(sizeof(char));
      while(strcmp(buffer,"fin")!=0){
        getmessage(sockfd,buffer);

      }
    }

    //REMEMBER TO CLOSE SOCKETS
    for(int i=0;i<nbjoueurs;i++){
      if(tabjoueurs[i].ordi==0){
        close(tabjoueurs[i].sockfd);
      }
    }
    close(sockfd);
  }

  terminate(plateau,rose,N);
  return (0);
}
