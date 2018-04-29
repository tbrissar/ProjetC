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

  void jeu(int N, int nbjoueurs, joueur *tabjoueurs, fleche *rose, cellule **plateau, int modejeu)
  {
    //printf("JEU\n");
    int cpt=0,tour=0,res=0;

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

  void init(int *nbjoueurs, joueur **tabjoueurs, fleche **rose, int *N, cellule ***plateau)
  {
    //printf("INIT\n");
    *rose = initrose();
    *tabjoueurs=initJoueurs(nbjoueurs);
    *plateau=initplateau(N,*nbjoueurs,*tabjoueurs,*rose);
  }

  void server(joueur *tabjoueurs, int nbjoueurs, int N, fleche *rose, cellule **plateau, int modejeu)
  {
    //printf("SERVER\n");
    int *tabsock;

    tabsock=connectionserver(tabjoueurs,nbjoueurs);
    broadcast("clear",nbjoueurs,tabjoueurs);
    broadcast("La partie commence\n",nbjoueurs,tabjoueurs);
    system(sleepslow);
    jeu(N,nbjoueurs,tabjoueurs,rose,plateau,modejeu);
    for(int i=0;i<nbjoueurs+1;i++){
      close(tabsock[i]);
    }
    free(tabsock);
    //printf("FIN SERVER\n");
  }

  void client()
  {
    //printf("CLIENT\n");
    int sockfd;
    int fin=0,x,y;

    sockfd=connectionclient();
    char *buffer=calloc(10,sizeof(char));

    while(fin==0){
      getmessage(sockfd,&buffer);

      //on saisie une case
      if(strcmp(buffer,"pose")==0){
        getmessage(sockfd,&buffer);
        printf("%s",buffer);
        do{
          buffer=realloc(buffer,sizeof(int)*3);
          scanf("%d,%d",&x,&y);
          memset(buffer,0,sizeof(int)*3);
          sprintf(buffer,"%d",x);
          sendmessage(sockfd,buffer);
          memset(buffer,0,sizeof(int)*3);
          sprintf(buffer,"%d",y);
          sendmessage(sockfd,buffer);
          getmessage(sockfd,&buffer);
        }while(strcmp(buffer,"poseok")!=0);
      }else if(strcmp(buffer,"clear")==0){
        system(clear);
      }else if(strcmp(buffer,"fin")==0){
        fin=1;
        printf("Partie terminée\n");
      }else{
        printf("%s",buffer);
      }
    }

    free(buffer);
    //REMEMBER TO CLOSE SOCKETS
    close(sockfd);
    //printf("FIN CLIENT\n");
  }

  cellule **plateau=NULL;
  fleche *rose=NULL;
  int N=0,nbjoueurs=-1,multirole=-1,n,res=1;
  // n: return value for write() and read()
  joueur *tabjoueurs=NULL;

  int sockfd,modejeu=-1;

int main()
{
  system(clear);
  while(modejeu!=0 && modejeu!=1){
    printf("Local:0 Reseau:1\n");
    scanf("%d",&modejeu);
  }


///////////////////////////////////////

  if(modejeu==0){

  //PARTIE EN LOCAL

  init(&nbjoueurs,&tabjoueurs,&rose,&N,&plateau);

  for(int i=0;i<nbjoueurs && res>0;i++){
    res=fork();
  }
  if(res==0){
    system("gnome-terminal");
    system(sleepconnection);
    client();
  }else if(res>0){
    server(tabjoueurs,nbjoueurs,N,rose,plateau,modejeu);
  }else{
    error("ERREUR fork()");
  }


/////////////////////////////////////

  }else if(modejeu==1){

  //PARTIE EN RESEAU

    while(multirole!=1 && multirole !=0){
      printf("Hote:1 Client:0\n");
      scanf("%d",&multirole);
    }

    //server
    if(multirole==1){
      init(&nbjoueurs,&tabjoueurs,&rose,&N,&plateau);
      server(tabjoueurs,nbjoueurs,N,rose,plateau,modejeu);
    //client
    }else if(multirole==0){
      client();
    }
  }

  terminate(plateau,rose,N,tabjoueurs);
  return (0);
}
