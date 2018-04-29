#!/bin/bash

clear

gcc -g -Wall bombes.c initreversi.c multi.c reversi.c traitre.c -o server server.c
gcc -g -Wall bombes.c initreversi.c multi.c reversi.c traitre.c -o client client.c
gcc -g -Wall bombes.c initreversi.c multi.c reversi.c traitre.c -o reversi main.c
