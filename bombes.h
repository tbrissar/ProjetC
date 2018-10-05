#ifndef __BOMBES__
#define __BOMBES__

  void init3x3(cellule **plateau, fleche *rose, int x, int y, int N);
  void explosion(cellule **plateau, color coul, fleche *rose, int x, int y, int N, int nbjoueurs, joueur *tabjoueurs);

#endif
