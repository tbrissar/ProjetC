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
#include "reversi.h"

//http://www.linuxhowtos.org/C_C++/socket.htm

void error(const char *msg)
{
  perror(msg);
  exit(1);
}

void sendmessage(int sockfd, char *message)
{
  long int messagelength=strlen(message);
  char messagelengthchar[21];
  sprintf(messagelengthchar,"%ld",messagelength);

  //sending the length of the message
  printf("Sending length: %ld\n",messagelength);
  if(write(sockfd,messagelengthchar,strlen(messagelengthchar))<0){
    error("ERROR writing messagelength to socket\n");
  }
  system(sleepsend);

  //sending the message
  printf("Sending: %s\n",message);
  if(write(sockfd,message,messagelength)<0){
    error("ERROR writing message to socket\n");
  }
  system(sleepsend);
}

void broadcast(char *message, int nbjoueurs, joueur *tabjoueurs)
{
  for(int i=0;i<nbjoueurs;i++){
    sendmessage(tabjoueurs[i].sockfd,message);
  }
}

void getmessage(int newsockfd, char **buffer)
{
  char buffersizechar[21];	//The server reads characters from the socket connection into this buffer
  memset(buffersizechar,0,21);//innitialize the buffer to zeroes

  //read size of message
  if(read(newsockfd,&buffersizechar,21)<0){
    error("ERROR reading from socket");
  }
  int buffersize=atoi(buffersizechar);

  printf("Taille du message a recevoir:%d\n",buffersize);
  *buffer=realloc(*buffer,sizeof(char)*buffersize);
  if(*buffer==NULL){
    error("ERROR realloc");
  }

  memset(*buffer,0,buffersize);
  //read message
  printf("reading message\n");
  if(read(newsockfd,*buffer,buffersize)<0){
    error("ERROR reading from socket");
  }
  system(sleepread);
}

int *connectionserver(joueur *tabjoueurs, int nbjoueurs)
{
  printf("CONNECTION SERVER\n");

  char *messageconnection=calloc(10,sizeof(char));
  int *tabsock=calloc((nbjoueurs+1),sizeof(int));
  if(messageconnection==NULL || tabsock==NULL){
    free(messageconnection);
    free(tabsock);
    error("ERREUR ALLOCATION CONNEXION SERVER");
  }

  int sockfd,portno=2500;
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
  printf("Mon adresse est %s\n",adresse);
  memset(adresse,0,INET_ADDRSTRLEN);

  //binds a socket to an adress
  clock_t start=clock(),end;
  while(bind(sockfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr))<0){ //requires socket fd, adress to bind (POINTER) and size of the adress
   system(sleepconnection);
   end=clock();
   if((double)(end-start)/CLOCKS_PER_SEC > attente){
     error("ERROR binding");
   }
  }

  int ordi=-1,currentsockfd;

  for(int i=0;i<nbjoueurs;i++){
    listen(sockfd,5);//listen on the socket for connections, second arg must be set to 5
    clilen = sizeof(cli_addr);//client adress size
    //returns a new fd needed to communicate on this connection
    currentsockfd=tabsock[i]=tabjoueurs[i].sockfd = accept(sockfd,(struct sockaddr *)&cli_addr,&clilen);
    if (currentsockfd<0){
      error("ERROR on accept");
    }
    //connection established as server

    getmessage(currentsockfd,&messageconnection);
    ordi=atoi(messageconnection);
    tabjoueurs[i].ordi=ordi;
    if(ordi==0){
      tabjoueurs[i].joker=5;
    }
    printf("Connexion etablie avec %s\n",inet_ntop(AF_INET,&cli_addr.sin_addr,adresse,INET_ADDRSTRLEN));
  }
  tabsock[nbjoueurs]=sockfd;
  free(messageconnection);
  free(adresse);
  printf("FIN CONNEXION SERVER\n");
  return(tabsock);
}

int connectionclient()
{
  printf("CONNECTION CLIENT\n");
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

  char *messageconnection=calloc(1,sizeof(char));

  printf("Humain:0 Ordi:1\n");
  int ordi=-1;
  while(ordi!=0 && ordi!=1){
    scanf("%d",&ordi);
  }
  sprintf(messageconnection,"%d",ordi);
  sendmessage(sockfd,messageconnection);

  printf("Connection au server reussie\n");

  free(messageconnection);
  free(hostname);
  system(clear);
  printf("FIN CONNECTION CLIENT\n");

  return(sockfd);
}