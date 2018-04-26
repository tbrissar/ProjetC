
#ifndef __REVERSI__
#define __REVERSI__

  typedef enum{vert=1,rouge,bleu,orange,violet,jaune}color;
  typedef enum{vide=0,pion,bombe,trou}content;
  //un joueur a une couleur, un nombre de coups possibles ainsi que des joker
  typedef struct{
    color couleur;
    int joker;
    int nbcoups;
  }joueur;
  typedef struct{
    content contenu;
    color couleur;
    int age;
  }cellule;
  typedef struct{
    int hori;
    int verti;
  }direction;
  //permet d'associer un nombre de cases à une direction
  typedef struct{
    direction dir;
    int nbcases;
  }fleche;


  char getSymbole(color coul, content cont);
  void affichage(cellule **plateau, int tour, int N);
  int checkbords(int i, int j, direction dir, int N);
  int checkcapture(cellule **plateau, int x, int y, direction dir, color coul, int N);
  void capture(cellule **plateau, fleche *rose, int x, int y, color coul);
  int pose(cellule **plateau, fleche *rose, color coul, int tour, int N);
  int verifcouprestant(cellule **plateau, fleche *rose, color coul, int N);
  direction directioninverse(fleche *rose, direction dir);
  int checkfin(cellule **plateau, fleche *rose, joueur *tabjoueurs, int cpt, int N, int nbjoueurs);
  void scores(cellule **plateau, joueur *tabjoueurs, int N, int nbjoueurs);
  color couleuraleatoire();

#endif
