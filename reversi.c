#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> //to use hostent
#include <limits.h>
#include "reversi.h"
#include "bombes.h"
#include "traitre.h"
#include "multi.h"
#include "macros.h"

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

//affichage du plateau sur la sortie standard
void affichage(cellule **plateau, int tour, int N, int nbjoueurs, joueur *tabjoueurs)
{
  //printf("AFFICHAGE\n");
  char affiche[650]={0};

  sprintf(affiche,"\nTour : %d\n\n    ",tour);

  for(int k=0;k<N;k++){
    if(k<10){
    sprintf(affiche,"%s %d ",affiche,k);
    }else{
    sprintf(affiche,"%s%d ",affiche,k);
    }
  }
  strcat(affiche," y\n");
  for(int i=0;i<N;i++){
    sprintf(affiche,"%s %d ",affiche,i);
    if(i<10){
      strcat(affiche," ");
    }
    for(int j=0;j<N;j++){
      sprintf(affiche,"%s[%c]",affiche,getSymbole(plateau[i][j].couleur,plateau[i][j].contenu));
    }
    strcat(affiche,"\n");
  }
  strcat(affiche," x\n\n");

  sendToAll(affiche,nbjoueurs,tabjoueurs);
  printf("%s",affiche);
  //printf("AFFICHAGE FIN\n");
}

//renvoie si la case dans la direction passée est hors plateau
bool isEdge(int i, int j, direction dir, int N)
{
  if
  ((dir.hori==1 && dir.verti==1 && (i==N-1 || j==N-1)) //sudest
  ||(dir.hori==1 && dir.verti==-1 && (i==N-1 || j==0))  //nordest
  ||(dir.hori==-1 && dir.verti==1 && (i==0 || j==N-1))  //sudouest
  ||(dir.hori==-1 && dir.verti==-1 && (i==0 || j==0))    //nordouest
  ||(dir.hori==1 && dir.verti==0 && i==N-1)             //est
  ||(dir.hori==-1 && dir.verti==0 && i==0)               //ouest
  ||(dir.hori==0 && dir.verti==1 && j==N-1)             //sud
  ||(dir.hori==0 && dir.verti==-1 && j==0)){          //nord
  return(true);
  }
  return(false);
}

//renvoie le nombre de pion captures avec une pose dans une direction donnee
int checkcapture(cellule **plateau, int x, int y, direction dir, color coul, int N)
{
  int i=x,j=y;
  cellule nextCell;

  if(isEdge(i,j,dir,N))
  {
    return(0);
  }else{
    nextCell=(plateau[i+dir.hori][j+dir.verti]);
    if(nextCell.contenu!=pion || nextCell.couleur==coul){
      return(0);
    }
  }
  while(nextCell.couleur!=coul){
    i+=dir.hori;j+=dir.verti;
    if(isEdge(i,j,dir,N))
    {
      return(0);
    }else{
      nextCell=(plateau[i+dir.hori][j+dir.verti]);
      if(nextCell.contenu!=pion){
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
int pose(cellule **plateau, fleche *rose, joueur j, int tour, int N, int nbjoueurs, joueur *tabjoueurs)
{
  int x,y,s=0;
  content cont=pion;
  coord bestcoord;
  color coul=j.couleur;
  char symbolejoueur=getSymbole(coul,cont);
  char *buffer=calloc(100,sizeof(char));

  sprintf(buffer,"C'est a %c de jouer\n",symbolejoueur);
  sendToAll(buffer,nbjoueurs,tabjoueurs);

  bestcoord=verifcouprestant(plateau,rose,j,N);
  if(bestcoord.coordx==-1){
    sendToAll("clear",nbjoueurs,tabjoueurs);
    sprintf(buffer,"%c ne peux pas jouer\n",symbolejoueur);
    sendToAll(buffer,nbjoueurs,tabjoueurs);
    trahison(plateau,rose,tour,N,nbjoueurs,tabjoueurs);
    free(buffer);
    return(0);
  }

  if(j.ordi==1){
    x=bestcoord.coordx;
    y=bestcoord.coordy;
    for(int i=0;i<8;i++){
      rose[i].nbcases=checkcapture(plateau,bestcoord.coordx,bestcoord.coordy,rose[i].dir,coul,N);
      s+=rose[i].nbcases;
    }
  }else{
    sendMessage(j.sockfd,"pose");
    sprintf(buffer,"Entrez la case ou vous souhaitez jouer %c (au format x,y)\n",symbolejoueur);
    sendMessage(j.sockfd,buffer);
    while(s==0){
      getMessage(j.sockfd,&buffer);
      x=atoi(buffer);
      getMessage(j.sockfd,&buffer);
      y=atoi(buffer);
      for(int i=0;i<8;i++){
        rose[i].nbcases=checkcapture(plateau,x,y,rose[i].dir,coul,N);
        s+=rose[i].nbcases;
      }
      if(s==0){
        sendMessage(j.sockfd,"posepasok");
      }
    }
    sendMessage(j.sockfd,"poseok");
  }

  //system(sleepslow);
  sendToAll("clear",nbjoueurs,tabjoueurs);
  if(plateau[x][y].contenu==bombe){
    explosion(plateau,coul,rose,x,y,N,nbjoueurs,tabjoueurs);
  }else{
    capture(plateau,rose,x,y,coul);
  }

  trahison(plateau,rose,tour,N,nbjoueurs,tabjoueurs);

  free(buffer);
  return(1);
}

//renvoie -1 en coordonnées si le joueur a au moins un coup jouable
coord verifcouprestant(cellule **plateau, fleche *rose, joueur j, int N)
{
  direction dir,dirinverse;
  cellule cell;
  int x,y,bestx,besty,bestmove=0,currentmove;
  coord coordcoup={-1,-1};
  color coul=j.couleur;

  for(int i=0;i<N;i++){
    for(int j=0;j<N;j++){
      cell=plateau[i][j];
      if(cell.contenu==pion && cell.couleur!=coul){
        //  printf("J'ai trouve un ennemi en %d,%d\n",i,j);
        for(int k=0;k<8;k++){
          dir=rose[k].dir;
          if(!isEdge(i,j,dir,N)){
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
    char *buffer=calloc(50,sizeof(char));
    sprintf(buffer,"Le meilleur coup immediat est en (%d,%d)\n",bestx,besty);
    sendMessage(j.sockfd,buffer);
    coordcoup.coordx=bestx;
    coordcoup.coordy=besty;
    free(buffer);
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
    if(verifcouprestant(plateau,rose,tabjoueurs[k],N).coordx!=-1){
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
  char *buffer=calloc(50,sizeof(char));

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
    sprintf(buffer,"Le joueur %c a un score de %d\n",getSymbole(coul,cont),score);
    sendToAll(buffer,nbjoueurs,tabjoueurs);
  }
  sendToAll("fin",nbjoueurs,tabjoueurs);
  free(buffer);
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
