#ifndef __MACROS__
#define __MACROS__

  #define ERREUR_GET_SYMBOLE 2
  #define ERREUR_COULEUR_ALEATOIRE 3
  #define ERREUR_ALLOCATION_MEMOIRE 1
  #define ERREUR_INIT_PLATEAU 4
  #define WRONG_INSTRUCTION 5

  #define modeaffichage 'D' //'D' displays bombs
  #define signal 4  //nombre de tour avant le debut des trahisons
  #define attente 60 //temps d'attente de connection en secondes

  //instructions from server to client
  #define instrpose "1"
  #define instrposeok "2"
  #define instrposepasok "3"
  #define instrclear "4"
  #define instrfin "5"
  #define instrtext "6"


  #ifdef _WIN32

  #define clear "cls"
  #define sleepread "CHOICE /T 0.5"
  #define sleepsend "CHOICE /T 1.5"
  #define sleepconnection "CHOICE /T 1"
  #define sleepslow "CHOICE /T 1"

  #elif __linux__

  #define clear "clear"
  #define sleepsend "sleep 0.1"
  #define sleepread "sleep 0.05"
  #define sleepconnection "sleep 1"
  #define sleepslow "sleep 1"

  #endif

#endif
