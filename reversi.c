#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include "reversi.h"
#include "bombes.h"
#include "traitre.h"

#define ERREUR_GET_SYMBOLE 2
#define ERREUR_COULEUR_ALEATOIRE 3
#define modeaffichage 'D'

//traduction du type case en caractere
char getSymbole(color coul, content cont)
{
  switch(cont){
    case bombe : if(modeaffichage=='D'){return('o');}else{return(' ');}
                break;
    case vide : return(' ');
                break;
    case trou : return('X');
                break;
    case pion : switch(coul){
                  case vert : return('1');break;
                  case rouge : return('2');break;
                  case bleu  : return('3');break;
                  case orange : return('4');break;
                  case violet : return('5');break;
                  case jaune : return('6');break;
    }
                break;
  }
  exit(ERREUR_GET_SYMBOLE);
}

//affichage d'une plateau sur la sortie standard
void affichage(cellule **plateau, int tour, int N)
{
  //system("clear");
  printf("\nTour : %d\n\n    ",tour);
  for(int k=0;k<N;k++){
    if(k<10){
    printf(" %d ",k);
  }else{
    printf("%d ",k);
  }
  }
  printf(" y\n");
  for(int i=0;i<N;i++){
    printf(" %d ",i);
    if(i<10){
      printf(" ");
    }
    for(int j=0;j<N;j++){
      printf("[%c]",getSymbole(plateau[i][j].couleur,plateau[i][j].contenu));
    }
    printf("\n");
  }
  printf(" x\n\n");
}

//pour verifier si l'on va sur un bord
//renvoie 0 si c'est le cas
int checkbords(int i, int j, direction dir, int N)
{
  if
  ((dir.hori==1 && dir.verti==1 && (i==N-1 || j==N-1)) //sudest
  ||(dir.hori==1 && dir.verti==-1 && (i==N-1 || j==0))  //nordest
  ||(dir.hori==-1 && dir.verti==1 && (i==0 || j==N-1))  //sudouest
  ||(dir.hori==-1 && dir.verti==-1 && (i==0 || j==0))    //nordouest
  ||(dir.hori==1 && dir.verti==0 && i==N-1)             //est
  ||(dir.hori==-1 && dir.verti==0 && i==0)               //ouest
  ||(dir.hori==0 && dir.verti==1 && j==N-1)             //sud
  ||(dir.hori==0 && dir.verti==-1 && j==0)){             //nord
    return(0);
  }
  return(1);
}

//renvoie le nombre de pion captures avec une pose dans une direction donnee
int checkcapture(cellule **plateau, int x, int y, direction dir, color coul, int N)
{
  int i=x,j=y;
  cellule suivante;

  if(!checkbords(i,j,dir,N))
  {
    return(0);
  }else{
    suivante=(plateau[i+dir.hori][j+dir.verti]);
    if(suivante.contenu!=pion || suivante.couleur==coul){
      return(0);
    }
  }
  while(suivante.couleur!=coul){
    i+=dir.hori;j+=dir.verti;
    if(!checkbords(i,j,dir,N))
    {
      return(0);
    }else{
      suivante=(plateau[i+dir.hori][j+dir.verti]);
      if(suivante.contenu!=pion){
        return(0);
      }
    }
  }
  if((i-x)!=0){
    return(abs(x-i));
  }else{
    return(abs(y-j));
  }
}

//capture de pions enemies
void capture(cellule **plateau, fleche *rose, int x, int y, color coul)
{
  int dir1,dir2;
  for(int i=0;i<8;i++){
    dir1=(rose[i].dir.hori);
    dir2=(rose[i].dir.verti);
    for(int j=0;j<=rose[i].nbcases;j++){
      plateau[x+dir1*j][y+dir2*j].couleur=coul;
      plateau[x+dir1*j][y+dir2*j].contenu=pion;
      plateau[x+dir1*j][y+dir2*j].age=1;
    }
  }
}

//pose d'un pion
//renvoie 0 si le joueur n'a pas pu jouer
int pose(cellule **plateau, fleche *rose, joueur j, int tour, int N, int nbjoueurs)
{
  int x,y,s=0;
  content cont=pion;
  coord bestcoord;
  color coul=j.couleur;
  char symbolejoueur=getSymbole(coul,cont);

  printf("C'est a %c de jouer\n",symbolejoueur);
  bestcoord=verifcouprestant(plateau,rose,coul,N);
  if(bestcoord.coordx==-1){
    system("clear");
    printf("%c ne peux pas jouer\n",symbolejoueur);
    trahison(plateau,rose,tour,N,nbjoueurs);
    return(0);
  }

  while(s==0){
    if(j.ordi==0){
    printf("Entrez la case ou vous souhaitez jouer %c (au format x,y)\n",symbolejoueur);
    scanf("%d,%d",&x,&y);
  }else{
    x=bestcoord.coordx;
    y=bestcoord.coordy;
  }
    for(int i=0;i<8;i++){
      rose[i].nbcases=checkcapture(plateau,x,y,rose[i].dir,coul,N);
      s+=rose[i].nbcases;
    }
  }

  system("sleep 1");
  system("clear");
  printf("%d,%d\n",x,y);
  if(plateau[x][y].contenu==bombe){
    explosion(plateau,coul,rose,x,y,N);
  }else{
    capture(plateau,rose,x,y,coul);
  }

  trahison(plateau,rose,tour,N,nbjoueurs);

  return(1);
}

//renvoie -1 en coordonnées si le joueur a au moins un coup jouable
coord verifcouprestant(cellule **plateau, fleche *rose, color coul, int N)
{
  direction dir,dirinverse;
  cellule cell;
  int x,y,bestx,besty,bestmove=0,currentmove;
  coord coordcoup={-1,-1};

  for(int i=0;i<N;i++){
    for(int j=0;j<N;j++){
      cell=plateau[i][j];
      if(cell.contenu==pion && cell.couleur!=coul){
        //  printf("J'ai trouve un ennemi en %d,%d\n",i,j);
        for(int k=0;k<8;k++){
          dir=rose[k].dir;
          if(checkbords(i,j,dir,N)){
            cell=plateau[i+dir.hori][j+dir.verti];
            x=i+dir.hori;
            y=j+dir.verti;
            dirinverse=directioninverse(rose,dir);
            currentmove=checkcapture(plateau,x,y,dirinverse,coul,N);
            if((cell.contenu==vide || cell.contenu==bombe) && (currentmove>0)){
              if(currentmove>bestmove){
                bestmove=currentmove;
                bestx=x;besty=y;
              }
              //printf("De %d,%d je peux l'avoir\n",x,y);
            }
            //printf("Pas depuis %d,%d  ",x,y);
          }
        }
        printf("\n");
      }
    }
  }
  if(bestmove>0){
    printf("Le meilleur coup immediat est en %d,%d\n",bestx,besty);
    coordcoup.coordx=bestx;
    coordcoup.coordy=besty;
    return(coordcoup);
  }
  return(coordcoup);
}

//renvoie la direction inverse
direction directioninverse(fleche *rose, direction dir)
{
  direction dirinverse;

  dirinverse.hori=-(dir.hori);
  dirinverse.verti=-(dir.verti);

  return(dirinverse);
}

//renvoie le prochain l'indice du prochain joueur pouvant joueur
//renvoie -1 s'il n'y en a aucun
int checkfin(cellule **plateau, fleche *rose, joueur *tabjoueurs, int cpt, int N, int nbjoueurs)
{
  for(int k=cpt;k<nbjoueurs;k++,(cpt++)%2){
    if(verifcouprestant(plateau,rose,tabjoueurs[k].couleur,N).coordx!=-1){
      return(k);
    };
  }
  return(-1);
}

//affiche le score
void scores(cellule **plateau, joueur *tabjoueurs, int N, int nbjoueurs)
{
  int score;
  color coul;
  content cont=pion;
  cellule cell;

  for(int k=0;k<nbjoueurs;k++){
    coul=tabjoueurs[k].couleur;
    score=0;
    for(int i=0;i<N;i++){
      for(int j=0;j<N;j++){
        cell=plateau[i][j];
        if(cell.contenu==pion && cell.couleur==coul)
          score++;
      }
    }
    printf("Le joueur %c a un score de %d\n",getSymbole(coul,cont),score);
  }
}

color couleuraleatoire()
{
  switch(rand()%6){
    case 0 : return(vert);
    case 1 : return(rouge);
    case 2 : return(bleu);
    case 3 : return(orange);
    case 4 : return(violet);
    case 5 : return(jaune);
  }
  exit(ERREUR_COULEUR_ALEATOIRE);
}
