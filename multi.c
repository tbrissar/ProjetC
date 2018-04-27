#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> //to use hostent
#include "reversi.h"

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void sendmessage(int sockfd, char *message)
{
  long int messagelengthint=strlen(message);
  char messagelengthchar[21];
  sprintf(messagelengthchar,"%ld",messagelengthint);

  printf("taille de la taille : %ld\n",strlen(messagelengthchar));
  //sending the length of the message
  if(write(sockfd,messagelengthchar,strlen(messagelengthchar))<0){
    error("ERROR writing messagelength to socket\n");
  }

  printf("Sending: %s\nOf length: %ld\n",message,messagelengthint);
  //sending the message
  if(write(sockfd,message,messagelengthint)<0){
    error("ERROR writing message to socket\n");
  }
}

void broadcast(char *message, int nbjoueurs, joueur *tabjoueurs)
{
  for(int i=0;i<nbjoueurs;i++){
    if(tabjoueurs[i].ordi==0){
      sendmessage(tabjoueurs[i].sockfd,message);
    }
  }
}

void getmessage(int newsockfd, char* buffer)
{
  char buffersizechar[21];	//The server reads characters from the socket connection into this buffer
  bzero(&buffersizechar,21);//innitialize the buffer to zeroes

  //read size of message
  if(read(newsockfd,&buffersizechar,21)<0){
    error("ERROR reading from socket");
  }
  int buffersize=atoi(buffersizechar);
  printf("Longueur du message a venir: %d\n",buffersize);
  if(realloc(buffer,sizeof(char)*buffersize)==NULL){
    error("ERROR realloc");
  }
  bzero(buffer,buffersize);
  //printf("messagebzero:%s\nde longueur:%ld\n",buffer,strlen(buffer));
  //read message
  if(read(newsockfd,buffer,buffersize)<0){
    error("ERROR reading from socket");
  }
}

void connectionserver(joueur *tabjoueurs, int nbjoueurs)
{
  char *messageconnection=malloc(sizeof(char));
  int sockfd,portno=2500;
  socklen_t clilen;//adress size
  struct sockaddr_in serv_addr, cli_addr;//contains an internet adress

  sockfd = socket(AF_INET, SOCK_STREAM, 0);//creates the socket, return the socket fd AF_INET:adresse domain Sock_Stream:type of the socket, last arg must be set to 0
  if (sockfd<0){
     error("ERROR opening socket");
   }

   bzero((char*)&serv_addr,sizeof(serv_addr));//initializes serv_addr to zeros
   //printf("Entrez un numéro de port à utiliser\n");
   //scanf("%d",&portno);

   serv_addr.sin_family = AF_INET;//adress family
   serv_addr.sin_addr.s_addr = INADDR_ANY;//IP adress of the host=machine where server is running INADDR_ANY=constant to store this adress
   serv_addr.sin_port = htons(portno);//htons converts port number in host byte order to port number in network byte order
   //binds a socket to an adress
   if (bind(sockfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr))<0){ //requires socket fd, adress to bind (POINTER) and size of the adress
     error("ERROR on binding");//in case of failure
   }

   for(int i=0;i<nbjoueurs;i++){
     if(tabjoueurs[i].ordi==0){
       listen(sockfd,5);//listen on the socket for connections, second arg must be set to 5
       clilen = sizeof(cli_addr);//client adress size
       //returns a new fd needed to communicate on this connection
       tabjoueurs[i].sockfd = accept(sockfd,(struct sockaddr *)&cli_addr,&clilen);
       if (tabjoueurs[i].sockfd<0){
         error("ERROR on accept");
       }
       //connection established as server
       getmessage(tabjoueurs[i].sockfd,messageconnection);
       printf("%s\n",messageconnection);
       free(messageconnection);
     }
   }
}

int connectionclient()
{
  struct sockaddr_in serv_addr;//server adress
  struct hostent *server;
  char *hostname="localhost";
  int portno=2500,sockfd;

  //struct hostent=defines a host computer on the Internet
  //printf("Entrez le numero de port à utiliser (doit être le même que celui du server)\n");
  //scanf("%d",&portno);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0){
    error("ERROR opening socket");
  }
  //printf("Entrez le nom/adresse de l'hôte\n");
  //bzero(hostname,30);
  //scanf("%s",hostname);
  //takes name of host and returns a pointer to a hostent
  server = gethostbyname(hostname);
  if (server==NULL) {
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
  }

  //set the fields in serv_adress
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);
  serv_addr.sin_port = htons(portno);

  if (connect(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0){
    error("ERROR connecting");
  }
  char messageconnection[40];
  sprintf(messageconnection,"Connection etablie avec %s",server->h_name);
  //connection established as client
  sendmessage(sockfd,messageconnection);
  return(sockfd);
}
