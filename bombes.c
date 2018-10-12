#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "reversi.h"
#include "bombes.h"
#include "multi.h"

//vide les cases situées autour de la cellule passée en paramètre
void init3x3(cellule **plateau, fleche *rose, int x, int y, int N)
{
  direction dir;
  for(int i=0;i<8;i++){
    dir=rose[i].dir;
    if(!isEdge(x,y,dir,N)){
      plateau[x+dir.hori][y+dir.verti].contenu=vide;
      plateau[x+dir.hori][y+dir.verti].age=0;
    }
  }
}

//explosion d'une bombe a effet aleatoire
void explosion(cellule **plateau, color coul, fleche *rose, int x, int y, int N, int nbjoueurs, joueur *tabjoueurs)
{
  direction dir;
  cellule cell;
  color macoul=coul;
  srand(time(NULL));


  sendToAll("Bombe : ",nbjoueurs,tabjoueurs);
  switch(rand()%5){
    case 0 :
            //laser ultra puissant
            dir=rose[rand()%8].dir;
            int i=x,j=y;
            while(!isEdge(i,j,dir,N)){
              i+=dir.hori;
              j+=dir.verti;
              cell=plateau[i][j];
              if(cell.contenu==pion || cell.contenu==bombe){
                plateau[i][j].contenu=vide;
                plateau[i][j].age=0;
              }
            }
            plateau[x][y].contenu=pion;
            plateau[x][y].couleur=coul;
            plateau[x][y].age=1;
            sendToAll("Laser!\n",nbjoueurs,tabjoueurs);
            break;
    case 1 :
            //change la couleur du pion qui vient d'etre pose
            while(macoul==coul){
              macoul=randomColor();
            }
            plateau[x][y].couleur=macoul;
            plateau[x][y].contenu=pion;
            plateau[x][y].age=1;
            sendToAll("Changement de couleur!\n",nbjoueurs,tabjoueurs);
            break;
    case 2 :
            //seul reste le pion joue
            init3x3(plateau,rose,x,y,N);
            plateau[x][y].couleur=coul;
            plateau[x][y].contenu=pion;
            plateau[x][y].age=1;
            sendToAll("Survivant!\n",nbjoueurs,tabjoueurs);
            break;
    case 3 :
            //explosion normale + case inutilisable
            init3x3(plateau,rose,x,y,N);
            plateau[x][y].contenu=trou;
            plateau[x][y].age=0;
            sendToAll("Mayhem!\n",nbjoueurs,tabjoueurs);
            break;
    case 4 :
            //explosion normale
            init3x3(plateau,rose,x,y,N);
            plateau[x][y].contenu=vide;
            plateau[x][y].age=0;
            sendToAll("Classique!\n",nbjoueurs,tabjoueurs);
            break;
    default :
            error("explosion() : valeur aleatoire incorrecte");
  }
  printf("\n");
}
