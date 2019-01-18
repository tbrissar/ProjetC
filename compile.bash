#!/bin/bash

clear

gcc -g -std=c99 -Wall -Wconversion myassert.c bombes.c initreversi.c multi.c reversi.c traitre.c -o server server.c
gcc -g -std=c99 -Wall -Wconversion myassert.c bombes.c initreversi.c multi.c reversi.c traitre.c -o client client.c
gcc -g -std=c99 -Wall -Wconversion myassert.c bombes.c initreversi.c multi.c reversi.c traitre.c -o reversi main.c
