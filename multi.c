#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> //to use hostent
#include <time.h>
#include <arpa/inet.h>
#include "macros.h"
#include "reversi.h"
#include "multi.h"

//http://www.linuxhowtos.org/C_C++/socket.htm

void error(const char *msg)
{
  perror(msg);
  exit(1);
}

////////////////////////////////////////
//            COMMUNICATION           //
////////////////////////////////////////

//forward declaration of getMessage()
//void getMessage(int newsockfd, char **buffer);

//send a message to socket sockfd
void sendMessage(int sockfd, char *message)
{
  //+1 to take \0 into account
  unsigned int messageLength=strlen(message)+1;

  //sending the length of the message
  if(write(sockfd,&messageLength,sizeof(unsigned int))<0){
    error("ERROR writing messageLength to socket\n");
  }

  //system(sleepsend);

  //sending the message
  if(write(sockfd,message,messageLength)<0){
    error("ERROR writing message to socket\n");
  }
  //system(sleepsend);
}

//send a message to all players
void sendToAll(char *message, int nbjoueurs, joueur *tabjoueurs)
{
  for(int i=0;i<nbjoueurs;i++){
    sendMessage(tabjoueurs[i].sockfd,message);
  }
}

//wait for a message from socket newsockfd
void getMessage(int newsockfd, char **buffer)
{
  unsigned int bufferLength;

  //read size of the message
  if(read(newsockfd,&bufferLength,sizeof(unsigned int))<0){
    error("ERROR reading from socket");
  }

  *buffer=realloc(*buffer,sizeof(char)*(bufferLength));
  if(*buffer==NULL){
    error("ERROR realloc");
  }

  //system(sleepread);

  //read message
  if(read(newsockfd,*buffer,bufferLength)<0){
    error("ERROR reading from socket");
  }
  //system(sleepread);
}


////////////////////////////////////////
//               SERVER               //
////////////////////////////////////////


int *connectionserver(joueur *tabjoueurs, int nbjoueurs, char *modejeu)
{
  //printf("CONNEXION SERVER\n");

  char *messageconnection=calloc(10,sizeof(char));
  int *tabsock=calloc((nbjoueurs+1),sizeof(int));
  if(messageconnection==NULL || tabsock==NULL){
    free(messageconnection);
    free(tabsock);
    error("ERREUR ALLOCATION CONNEXION SERVER");
  }

  int sockfd,portno=2500,timer=0,res=1;
  socklen_t clilen;//adress size
  struct sockaddr_in serv_addr, cli_addr;//contains an internet adress

  sockfd = socket(AF_INET, SOCK_STREAM, 0);//creates the socket, return the socket fd AF_INET:adresse domain Sock_Stream:type of the socket, last arg must be set to 0
  if (sockfd<0){
     error("ERROR opening socket");
  }

  memset((char*)&serv_addr,0,sizeof(serv_addr));//initializes serv_addr to zeros
  //printf("Entrez un numéro de port à utiliser\n");
  //scanf("%d",&portno);

  serv_addr.sin_family = AF_INET;//adress family
  serv_addr.sin_addr.s_addr = INADDR_ANY;//IP adress of the host=machine where server is running INADDR_ANY=constant to store this adress
  serv_addr.sin_port = htons(portno);//htons converts port number in host byte order to port number in network byte order

  char *adresse=calloc(INET_ADDRSTRLEN,sizeof(char));
  inet_ntop(AF_INET,&serv_addr.sin_addr,adresse,INET_ADDRSTRLEN);
  printf("Mon adresse est (A FAIRE)%s\n",adresse);
  memset(adresse,0,INET_ADDRSTRLEN);


  //binds a socket to an adress
  while(bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0){ //requires socket fd, adress to bind (POINTER) and size of the adress
   system(sleepconnection);
   timer++;
   if(timer>attente){
     error("ERROR binding");
   }
  }

  int ordi=-1,currentsockfd;

  for(int i=0;i<nbjoueurs && res>0 ;i++){

    //si en local, le serveur cree lui-meme les clients
    if(strcmp(modejeu,"local")==0){
      res=fork();
      if(res==0){
        system(sleepslow);
        system("xterm -e ./client localhost");
        exit(0);
      }
    }

    listen(sockfd,5);//listen on the socket for connections, second arg must be set to 5
    clilen = sizeof(cli_addr);//client adress size
    //returns a new fd needed to communicate on this connection
    currentsockfd=tabsock[i]=tabjoueurs[i].sockfd = accept(sockfd,(struct sockaddr *)&cli_addr,&clilen);
    if (currentsockfd<0){
      error("ERROR on accept");
    }
    //connection established as server

    getMessage(currentsockfd,&messageconnection);
    ordi=atoi(messageconnection);
    tabjoueurs[i].ordi=ordi;
    printf("Connexion etablie avec %s\n",inet_ntop(AF_INET,&cli_addr.sin_addr,adresse,INET_ADDRSTRLEN));
  }

  tabsock[nbjoueurs]=sockfd;
  free(messageconnection);
  free(adresse);
  //printf("FIN CONNEXION SERVER\n");
  return(tabsock);
}


////////////////////////////////////////
//               CLIENT               //
////////////////////////////////////////


int connectionclient(char *adress)
{
  //printf("CONNEXION CLIENT\n");
  struct sockaddr_in serv_addr;//server adress
  struct hostent *server=NULL;
  char *hostname=calloc(30,sizeof(char));
  int portno=2500,sockfd;

  //struct hostent=defines a host computer on the Internet
  //printf("Entrez le numero de port à utiliser (doit être le même que celui utilisé par le serveur)\n");
  //scanf("%d",&portno);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0){
    error("ERROR opening socket");
  }

  strcpy(hostname,adress);
  server=gethostbyname(hostname);

  while(server==NULL){
    printf("Entrez le nom/adresse de l'hôte\n");
    scanf("%s",hostname);
    //takes name of host and returns a pointer to a hostent
    server=gethostbyname(hostname);
  }

  //set the fields in serv_adress
  memset((char *) &serv_addr,0,sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
  serv_addr.sin_port = htons(portno);

  clock_t start=clock(),end;
  while(connect(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
  {
    system(sleepconnection);
    end=clock();
    if((double)(end-start)/CLOCKS_PER_SEC > attente){
      error("ERROR connecting");
    }
  }
  //connection established as client

  char *messageconnection=calloc(10,sizeof(char));

  printf("Humain:0 Ordi:1\n");
  int ordi=-1;
  while(ordi!=0 && ordi!=1){
    scanf("%d",&ordi);
  }
  sprintf(messageconnection,"%d",ordi);
  sendMessage(sockfd,messageconnection);

  printf("Connexion au server reussie\n");

  free(messageconnection);
  free(hostname);
  //printf("FIN CONNEXION CLIENT\n");
  return(sockfd);
}
