#ifndef __INIT__
#define __INIT__

  joueur *initJoueurs(int *nbjoueurs);
  fleche *initrose();
  cellule **initplateau(int *N, int nbjoueurs, joueur *tabjoueurs, fleche *rose);
  int terminate(cellule **plateau, fleche *rose, int N, joueur *tabjoueurs);
  int affectationcouleur(int nbj, joueur *tabjoueurs);
  void depart2(cellule **plateau, int x, int y, color a, color b);
  void depart3(cellule **plateau, int x, int y, color a, color b, color c);
  int ras(cellule **plateau, int x, int y, fleche *rose, int N);

#endif
