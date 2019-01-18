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
  //+1 to take \0 into account
  size_t messageLength=strlen(message)+1;

  //sending the length of the message
  myassert(write(sockfd,&messageLength,sizeof(unsigned int)) < 0);

  //sending the message
  myassert(write(sockfd,message,messageLength) < 0);
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
  unsigned int bufferLength;

  //read size of the message
  myassert(read(newsockfd,&bufferLength,sizeof(unsigned int)) < 0);

  myassert((*buffer = realloc(*buffer,sizeof(char)*(bufferLength))) != NULL);

  //read message
  myassert(read(newsockfd,*buffer,bufferLength) < 0);
}


////////////////////////////////////////
//               SERVER               //
////////////////////////////////////////


int *connectionserver(joueur *tabjoueurs, int nbjoueurs, char *modejeu)
{
  printf("CONNEXION SERVER\n");

  char *messageconnection;
  int *tabsock;

  myassert((messageconnection = calloc(10, sizeof(char))) != NULL);
  myassert((tabsock = calloc((size_t)(nbjoueurs+1), sizeof(int))) != NULL);

  int sockfd,timer=0,res=1;
  uint16_t portno=2500;
  socklen_t clilen;//adress size
  struct sockaddr_in serv_addr, cli_addr;//contains an internet adress

  //creates the socket, return the socket fd AF_INET:adresse domain Sock_Stream:type of the socket, last arg must be set to 0
  myassert(sockfd = socket(AF_INET, SOCK_STREAM, 0) < 0);

  memset((char*)&serv_addr, 0, sizeof(serv_addr));//initializes serv_addr to zeros
  //printf("Entrez un numéro de port à utiliser\n");
  //scanf("%d",&portno);

  serv_addr.sin_family = AF_INET;//adress family
  serv_addr.sin_addr.s_addr = INADDR_ANY;//IP adress of the host=machine where server is running INADDR_ANY=constant to store this adress
  serv_addr.sin_port = htons(portno);//htons converts port number in host byte order to port number in network byte order

  char *adresse = calloc(INET_ADDRSTRLEN, sizeof(char));
  inet_ntop(AF_INET, &serv_addr.sin_addr, adresse, INET_ADDRSTRLEN);
  printf("Mon adresse est (A FAIRE)%s\n", adresse);
  memset(adresse, 0, INET_ADDRSTRLEN);


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
	IPbuffer = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));

	printf("Hostname: %s\n", hostbuffer);
	printf("Host IP: %s\n", IPbuffer);
//////////////////////////////////////////


  //binds a socket to an adress
  while(bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){ //requires socket fd, adress to bind (POINTER) and size of the adress
   system(sleepconnection);
   timer++;
   if(timer>attente){
     error("ERROR timeout binding");
   }
  }

  int ordi=-1,currentsockfd;

  for(int i = 0; i < nbjoueurs && res > 0; i++){

    //si en local, le serveur cree lui-meme les clients
    if(strcmp(modejeu,"local") == 0){
      res=fork();
      if(res == 0){
        system(sleepslow);
        system("xterm -e ./client localhost");
        exit(0);
      }
    }

    listen(sockfd,5);//listen on the socket for connections, second arg must be set to 5
    clilen = sizeof(cli_addr);//client adress size
    //returns a new fd needed to communicate on this connection
    currentsockfd = tabsock[i] = tabjoueurs[i].sockfd = accept(sockfd,(struct sockaddr *)&cli_addr,&clilen);
    myassert(currentsockfd < 0);
    //connection established as server

    getMessage(currentsockfd,&messageconnection);
    ordi = atoi(messageconnection);
    tabjoueurs[i].ordi = ordi;
    printf("Connexion etablie avec %s\n",inet_ntop(AF_INET, &cli_addr.sin_addr, adresse, INET_ADDRSTRLEN));
  }

  tabsock[nbjoueurs] = sockfd;
  free(messageconnection);
  free(adresse);

  printf("FIN CONNEXION SERVER\n");
  return tabsock;
}


////////////////////////////////////////
//               CLIENT               //
////////////////////////////////////////


int connectionclient(char *adress)
{
  printf("CONNEXION CLIENT\n");

  struct sockaddr_in serv_addr;//server adress
  struct hostent *server = NULL;
  int sockfd;
  uint16_t portno = 2500;

  //struct hostent=defines a host computer on the Internet
  //printf("Entrez le numero de port à utiliser (doit être le même que celui utilisé par le serveur)\n");
  //scanf("%d",&portno);

  myassert(sockfd = socket(AF_INET, SOCK_STREAM, 0) < 0);

  char *hostname = calloc(30,sizeof(char));
  myassert(hostname != NULL);
  hostname = strcpy(hostname,adress);
  myassert(hostname != NULL);
  server = gethostbyname(hostname);

  do{
    printf("Entrez le nom/adresse de l'hôte\n");
    scanf("%s",hostname);
    //takes name of host and returns a pointer to a hostent
    server = gethostbyname(hostname);
  }while(server == NULL);

  //set the fields in serv_adress
  memset((char *) &serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, (size_t)server->h_length);
  serv_addr.sin_port = htons(portno);

  clock_t start = clock();
  while(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
  {
    system(sleepconnection);
    if((double)(clock() - start)/CLOCKS_PER_SEC > attente){
      error("ERROR connecting");
    }
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

  printf("FIN CONNEXION CLIENT\n");
  return sockfd;
}
