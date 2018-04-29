#ifndef __MULTI__
#define __MULTI__

  #include "reversi.h"

  int connectionclient(char *adress);
  int *connectionserver(joueur *tabjoueurs, int nbjoueurs, char *modejeu);
  void getmessage(int newsockfd, char **buffer);
  void sendmessage(int newsockfd, char *message);
  void error(const char *msg);
  void broadcast(char *message, int nbjoueurs, joueur *tabjoueurs);

#endif
