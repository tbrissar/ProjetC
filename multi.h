#ifndef __MULTI__
#define __MULTI__

  int connectionclient();
  int *connectionserver(joueur *tabjoueurs, int nbjoueurs);
  void getmessage(int newsockfd, char **buffer);
  void sendmessage(int newsockfd, char *message);
  void error(const char *msg);
  void broadcast(char *message, int nbjoueurs, joueur *tabjoueurs);

#endif
