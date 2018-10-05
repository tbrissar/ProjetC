#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "macros.h"
#include "multi.h"

  int fin=0,x,y,sockfd;

int main(int argc, char *argv[])
{
  sockfd=connectionclient(argv[1]);
  char *buffer=calloc(10,sizeof(char));
  char *checkfin=calloc(4,sizeof(char));

  while(fin==0){
    getmessage(sockfd,&buffer);

    if(strcmp(buffer,"pose")==0){
      //saisie de case
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
      //effacement du contenu de la console
      system(clear);

    }else if(strcmp(buffer,"fin")==0){
      //partie terminée
      fin=1;
      printf("Partie terminée\nEcrivez fin pour quitter\n");
      do{
        scanf("%s",checkfin);
      }while(strcmp(checkfin,"fin")!=0);
    }else{
      //affichage du message receptione
      printf("%s",buffer);
    }
  }
  free(buffer);

  sendmessage(sockfd,"termine");
  //fermeture du socket cote client
  close(sockfd);

  return(0);
}
