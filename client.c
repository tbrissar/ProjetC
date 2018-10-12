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
    getMessage(sockfd,&buffer);
    int instruction=(int)strtol(buffer,NULL,10);

    switch(instruction){

    case 1:
      getMessage(sockfd,&buffer);
      printf("%s",buffer);
      do{
        buffer=realloc(buffer,sizeof(int)*3);
        scanf("%d,%d",&x,&y);
        sprintf(buffer,"%d",x);
        sendMessage(sockfd,buffer);
        sprintf(buffer,"%d",y);
        sendMessage(sockfd,buffer);
        getMessage(sockfd,&buffer);
        instruction=(int)strtol(buffer,NULL,10);
      }while(instruction!=2);
      break;

    case 4:
      //display clearing
      system(clear);
      break;

    case 5:
      //partie terminée
      fin=1;
      printf("Partie terminée\nEcrivez fin pour quitter\n");
      do{
        scanf("%s",checkfin);
      }while(instruction!=5);
      break;

    case 6:
      //displaying plain text
      getMessage(sockfd,&buffer);
      printf("%s",buffer);
      break;

    default:
      printf("WRONG_INSTRUCTION\n");
      break;

    }
  }

  free(buffer);

  sendMessage(sockfd,"termine");
  //fermeture du socket cote client
  close(sockfd);

  return(0);
}
