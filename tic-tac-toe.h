//Tic-tac-toe playing AI. Exhaustive tree-search. WTFPL
//Matthew Steel 2009, www.www.repsilat.com

#include <stdio.h>

int board[9] = {0,0,0,0,0,0,0,0,0};
int player;
unsigned turn;

char gridChar(int i);

void draw(int b[9]);

int win(const int board[9]);

int minimax(int board[9], int player);

void computerMove(int board[9]);

void player_move(int board[9]);

void showFunGame(char *user, int player);

int handleTicTacToe();