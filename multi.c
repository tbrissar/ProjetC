#define _DEFAULT_SOURCE

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
#include "myassert.h"

//http://www.linuxhowtos.org/C_C++/socket.htm

////////////////////////////////////////
//            COMMUNICATION           //
////////////////////////////////////////

//forward declaration of getMessage()
//void getMessage(int newsockfd, char **buffer);

//send a message to socket sockfd
void sendMessage(int sockfd, char *message)
{
  size_t messageLength = strlen(message);

  //sending the length of the message
  ssize_t resWrite = write(sockfd, &messageLength, sizeof(size_t));
  myassert(resWrite != -1);

  //sending the message
  resWrite = write(sockfd, message, messageLength);
  myassert(resWrite != -1);
}

//send a message to all players
void sendToAll(char *message, int nbjoueurs, joueur *tabjoueurs)
{
  for(int i = 0;i < nbjoueurs;i++){
    sendMessage(tabjoueurs[i].sockfd,message);
  }
}

//wait for a message from socket newsockfd
void getMessage(int newsockfd, char **buffer)
{
  size_t bufferLength;

  //read size of the message
  ssize_t resRead = read(newsockfd, &bufferLength, sizeof(size_t));
  myassert(resRead != -1);

  *buffer = realloc(*buffer,sizeof(char) * (bufferLength + 1));
  myassert(buffer != NULL);

  //read message
  resRead = read(newsockfd, *buffer, bufferLength);
  myassert(resRead != -1);
}


////////////////////////////////////////
//               SERVER               //
////////////////////////////////////////


int *connectionserver(joueur *tabjoueurs, int nbjoueurs, char *modejeu)
{
  printf("CONNEXION SERVER\n");

  char *messageconnection;
  int *tabsock;

  messageconnection = calloc(10, sizeof(char));
  myassert(messageconnection != NULL);
  tabsock = calloc((size_t)(nbjoueurs+1), sizeof(int));
  myassert(tabsock != NULL);

  int sockfd, res = 1;
  uint16_t portno = 2500;
  socklen_t clilen;//adress size
  struct sockaddr_in serv_addr, cli_addr;//contains an internet adress

  //creates the socket, return the socket fd AF_INET:adresse domain Sock_Stream:type of the socket, last arg must be set to 0
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  myassert(sockfd != -1);

  memset((char*)&serv_addr, 0, sizeof(serv_addr));//initializes serv_addr to zeros
  //printf("Entrez un numéro de port à utiliser\n");
  //scanf("%d",&portno);

  serv_addr.sin_family = AF_INET;//adress family
  serv_addr.sin_addr.s_addr = INADDR_ANY;//IP adress of the host=machine where server is running INADDR_ANY=constant to store this adress
  serv_addr.sin_port = htons(portno);//htons converts port number in host byte order to port number in network byte order

///////////////////////////////////////////
  char hostbuffer[256];
	char *IPbuffer;
	struct hostent *host_entry;

	// To retrieve hostname
	myassert(gethostname(hostbuffer, sizeof(hostbuffer)) != -1);

	// To retrieve host information
	host_entry = gethostbyname(hostbuffer);

	// To convert an Internet network
	// address into ASCII string
	IPbuffer = inet_ntoa(*((struct in_addr*)host_entry -> h_addr_list[0]));

	printf("Host IP: %s\n", IPbuffer);
//////////////////////////////////////////


  //binds a socket to an adress
  //requires socket fd, adress to bind (POINTER) and size of the adress
  int resBind = bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
  myassert(resBind != -1);

  int ordi = -1, currentsockfd;

  for(int i = 0; i < nbjoueurs && res > 0; i++){
    //si en local, le serveur cree lui-meme les clients
    if(strcmp(modejeu,"local") == 0){
      res = fork();
      myassert(res != -1);
      if(res == 0){
        myassert(system("xterm -e ./client localhost") != -1);
        exit(0);
      }
    }

    listen(sockfd,5);//listen on the socket for connections, second arg must be set to 5
    clilen = sizeof(cli_addr);//client adress size
    //returns a new fd needed to communicate on this connection
    currentsockfd = tabsock[i] = tabjoueurs[i].sockfd = accept(sockfd,(struct sockaddr *)&cli_addr,&clilen);
    myassert(currentsockfd!= -1);
    //connection established as server

    getMessage(currentsockfd,&messageconnection);
    ordi = atoi(messageconnection);
    tabjoueurs[i].ordi = ordi;

    char *clientAdresseString = calloc (INET_ADDRSTRLEN, sizeof(char));
    clientAdresseString = (char *)inet_ntop(AF_INET, &cli_addr.sin_addr, clientAdresseString, INET_ADDRSTRLEN);
    myassert(clientAdresseString != NULL);
    printf("Connexion etablie avec %s\n",clientAdresseString);
    free(clientAdresseString);
  }

  tabsock[nbjoueurs] = sockfd;
  free(messageconnection);

  printf("FIN CONNEXION SERVER\n");
  return tabsock;
}


////////////////////////////////////////
//               CLIENT               //
////////////////////////////////////////


int connectionclient(char *adress)
{
  struct sockaddr_in serv_addr;//server adress
  struct hostent *server = NULL;
  int sockfd;
  uint16_t portno = 2500;

  //struct hostent=defines a host computer on the Internet
  //printf("Entrez le numero de port à utiliser (doit être le même que celui utilisé par le serveur)\n");
  //scanf("%d",&portno);

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  myassert(sockfd != -1);

  char *hostname = calloc(30,sizeof(char));
  myassert(hostname != NULL);
  hostname = strcpy(hostname,adress);
  myassert(hostname != NULL);
  server = gethostbyname(hostname);

  while(server == NULL){
    printf("Entrez le nom/adresse de l'hôte\n");
    scanf("%s",hostname);
    //takes name of host and returns a pointer to a hostent
    server = gethostbyname(hostname);
  }

  //set the fields in serv_adress
  memset((char *) &serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, (size_t)server->h_length);
  serv_addr.sin_port = htons(portno);

  clock_t start = clock();
  while(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
  {
    system(sleepconnection);
    if(((double)(clock() - start) / CLOCKS_PER_SEC) > attente) error("ERROR connecting");
  }
  //connection established as client

  char *messageconnection = calloc(10,sizeof(char));
  myassert(messageconnection != NULL);

  printf("Humain:0 Ordi:1\n");
  int ordi = -1;
  while(ordi != 0 && ordi != 1){
    scanf("%d",&ordi);
  }
  sprintf(messageconnection, "%d", ordi);
  sendMessage(sockfd, messageconnection);

  printf("Connexion au server reussie\n");

  free(messageconnection);
  free(hostname);

  return sockfd;
}
