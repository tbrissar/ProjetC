#include "reversi.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>


#define ERREUR_ALLOCATION_MEMOIRE 1
#define ERREUR_getSymbole 2
#define mode 'D'
//note : N pair et >= 6
#define N 10

//notre plateau
static cellule **grille;

//tableau de directions prédéfinies
const direction
nord={0,-1},
sud={0,1},est={1,0},
ouest={-1,0},
nordest={1,-1},
nordouest={-1,-1},
sudest={1,1},
sudouest={-1,1};

//à mettre en constante
fleche *rose;

//initialisation du tableau de directions
void initrose()
{
  printf("Entrée initrose()\n");
  rose=malloc(sizeof(fleche)*8);
  rose[0].dir=nord;
  rose[1].dir=sud;
  rose[2].dir=est;
  rose[3].dir=ouest;
  rose[4].dir=nordest;
  rose[5].dir=nordouest;
  rose[6].dir=sudest;
  rose[7].dir=sudouest;
  printf("Sortie initrose()\n");
}

//initialisation de la grille de jeu
void initGrille(int M)
{
  printf("Entrée initGrille()\n");
  int randomX,randomY,milieu=(N/2)-1;
  srand(time(NULL));

  initrose();

  grille=malloc(N*sizeof(cellule*));
  if(grille==NULL){
    exit(ERREUR_ALLOCATION_MEMOIRE);
  }

  for(int i=0;i<N;i++){
    grille[i]=malloc(N*sizeof(cellule));
    if(grille[i]==NULL){
      for(int j=i-1;j>=0;j--){
        free(grille[j]);
      }
      free(grille);
      exit(ERREUR_ALLOCATION_MEMOIRE);
    }
  }

  for(int i=0;i<N;i++){
    for(int j=0;j<N;j++){
      grille[i][j]=vide;
    }
  }

  grille[milieu][milieu]=grille[milieu+1][milieu+1]=vert;
  grille[milieu+1][milieu]=grille[milieu][milieu+1]=rouge;

  for(int i=1;i<=M;i++){
    do{
      randomX=rand()%N;
      randomY=rand()%N;
    }while((((randomX >= milieu-1) && (randomX <= milieu+2)) && ((randomY >= milieu-1) && (randomY <= milieu+2))) || (grille[randomX][randomY]==bombe));
    grille[randomX][randomY]=bombe;
  }
  printf("Sortie initGrille()\n");
}

//désallouages
int terminate()
{
  for(int i=N-1;i>=0;i--){
    free(grille[i]);
  }
  free(grille);
  free(rose);
  return(0);
}

//traduction du type case en caractère
char getSymbole(cellule c)
{
  switch (c) {
    case vide : return(' ');
    case bombe :
    if(mode=='J'){
      return(' ');
    }else if(mode=='D'){
      return('o');
    }
    case vert : return('V');
    case rouge : return('R');
    default : exit(ERREUR_getSymbole);
  }
}

//affichage d'une grille sur la sortie standard
void affichage()
{
  system("clear");
  for(int i=0;i<N;i++){
    for(int j=0;j<N;j++){
      printf("[%c]",getSymbole(grille[i][j]));
    }
    printf("\n");
  }
}

//pour vérifier si l'on va sur un bord
//renvoie 0 si c'est le cas
int checkbords(int i, int j, direction dir)
{
  if
  ((dir.dirhori==1 && dir.dirverti==1 && (i==N-1 || j==N-1))  //sudest
  ||(dir.dirhori==1 && dir.dirverti==-1 && (i==N-1 || j==0))  //nordest
  ||(dir.dirhori==-1 && dir.dirverti==1 && (i==0 || j==N-1))  //sudouest
  ||(dir.dirhori==-1 && dir.dirverti==-1 && (i==0 || j==0))   //nordouest
  ||(dir.dirhori==1 && dir.dirverti==0 && i==N-1)             //est
  ||(dir.dirhori==-1 && dir.dirverti==0 && i==0)              //ouest
  ||(dir.dirhori==0 && dir.dirverti==1 && j==N-1)             //sud
  ||(dir.dirhori==0 && dir.dirverti==-1 && j==0)){            //nord
    return(0);
  }
  return(1);
}

//renvoie le nombre de pion capturés avec une pose dans une direction donnée
int checkcapture(int x, int y, direction dir, cellule c)
{
  int i=x,j=y;
  cellule suivante;

  if(!checkbords(i,j,dir))
  {
    return(0);
  }else{
    suivante=(grille[i+dir.dirhori][y+dir.dirverti]);
    if(suivante==vide || suivante==bombe || suivante==c){
      return(0);
    }
  }
  while(suivante!=c){
    i+=dir.dirhori;j+=dir.dirverti;
    if(!checkbords(i,j,dir))
    {
      return(0);
    }else{
      suivante=(grille[i+dir.dirhori][y+dir.dirverti]);
      if(suivante==vide || suivante==bombe){
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

//explosion d'une bombe, affecte vide à la case ainsi qu'à celles autour d'elle
void explosion(int x, int y)
{
  direction dir;
  for(int i=0;i<8;i++){
    dir=rose[i].dir;
    if(checkbords(x,y,dir)){
      grille[x+dir.dirhori][y+dir.dirverti]=vide;
    }
  }
  grille[x][y]=vide;
}

//capture de pions enemies
void capture(int x, int y, cellule c)
{int dir1,dir2;
  for(int i=0;i<8;i++){
    dir1=(rose[i].dir.dirhori);
    dir2=(rose[i].dir.dirverti);
    for(int j=1;j<rose[i].nbcases;j++){
      grille[x+dir1*j][y+dir2*j]=c;
    }
  }
}

//pose d'un pion
void pose(cellule c)
{
  int x,y,s;
  do{
    printf("Entrez la case où vous souhaitez jouer au format x,y\n");
    scanf("%d,%d",&x,&y);
    for(int i=0;i<8;i++){
      s+=rose[i].nbcases=checkcapture(x,y,rose[i].dir,c);
    }
  }while(!s);

  if(grille[x][y]==bombe){
    explosion(x,y);
  }else{
    capture(x,y,c);
  }
}
