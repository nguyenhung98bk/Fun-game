#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // clean screen

int player;
unsigned turn;

char gridChar(int i, char c) {
  switch(i) {
  case -1:
    return 'X';
  case 0:
    return c;
  case 1:
    return 'O';
  }
}

void draw(int b[9]) {
  printf("\033[0;33m");
  printf(" %c | %c | %c\n",gridChar(b[0], '1'),gridChar(b[1], '2'),gridChar(b[2], '3'));
  printf("---+---+---\n");
  printf(" %c | %c | %c\n",gridChar(b[3], '4'),gridChar(b[4], '5'),gridChar(b[5], '6'));
  printf("---+---+---\n");
  printf(" %c | %c | %c\n",gridChar(b[6], '7'),gridChar(b[7], '8'),gridChar(b[8], '9'));
}

int win(const int board[9]) {
  //determines if a player has won, returns 0 otherwise.
  unsigned wins[8][3] = {{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}};
  int i;
  for(i = 0; i < 8; ++i) {
    if(board[wins[i][0]] != 0 &&
      board[wins[i][0]] == board[wins[i][1]] &&
      board[wins[i][0]] == board[wins[i][2]])
        return board[wins[i][2]];
  }
  return 0;
}

int minimax(int board[9], int player) {
  //How is the position like for player (their turn) on board?
  int winner = win(board);
  if(winner != 0) return winner*player;

  int move = -1;
  int score = -2;//Losing moves are preferred to no move
  int i;
  for(i = 0; i < 9; ++i) {//For all moves,
    if(board[i] == 0) {//If legal,
      board[i] = player;//Try the move
      int thisScore = -minimax(board, player*-1);
      if(thisScore > score) {
        score = thisScore;
        move = i;
      }//Pick the one that's worst for the opponent
      board[i] = 0;//Reset board after try
    }
  }
  if(move == -1) return 0;
  return score;
}

void computerMove(int board[9]) {
  int move = -1;
  int score = -2;
  int i;
  for(i = 0; i < 9; ++i) {
    if(board[i] == 0) {
      board[i] = 1;
      int tempScore = -minimax(board, -1);
      board[i] = 0;
      if(tempScore > score) {
        score = tempScore;
        move = i;
      }
    }
  }
  //returns a score based on minimax tree at a given node.
  board[move] = 1;
}

void player_move(int board[9]) {
  int move = 0;
  do {
    printf("\033[0;37m");
    start:
    // printf("\nInput move ([0..8]): ");
    // scanf("%d", &move);
    printf("\nInput move ([1..9]): ");
    scanf("%d", &move);
    move = move -1;
    if(board[move] != 0) {
      printf("Its Already Occupied !");
      goto start;
    }
    printf("\n");
  } while (move >= 9 || move < 0 && board[move] == 0);

  board[move] = -1;
}

void showFunGame(char *user, int player){
  write(1,"\E[H\E[2J", 7);
  printf("\033[0;37m");
  printf("----------FUNGAME----------\n");
  printf("\tUsername: %s!\n", user);
  printf("\tNEW TicTacToe GAME\n");
  printf("---------------------------\n");
  printf("Computer: O, You: X\n");
  if(player == 1) printf("You play (1)st\n\n");
  else if( player == 2) printf("You play (2)nd\n\n");
}

int handleTicTacToe( char *user) {
  int board[9] = {0,0,0,0,0,0,0,0,0};
  //computer squares are 1, player squares are -1.
  printf("Computer: O, You: X\n");
  printf("You want to Play (1)st or (2)nd: ");
  player=0;
  scanf("%d",&player);
  printf("\n");
  
  for(turn = 0; turn < 9 && win(board) == 0; ++turn) {
    if( turn != 0 ) showFunGame(user, player);

    if((turn+player) % 2 == 0)
      computerMove(board);
    else {
      draw(board);
      player_move(board);
    }

    if( turn == 0 ) write(1,"\E[H\E[2J", 7);
  }

  switch(win(board)) {
    case 0:
      printf("\033[0;37m");
      printf("A draw. How droll.\n");
      return 0;
    case 1:
      draw(board);
      printf("\033[0;37m");
      printf("You lose.\n");
      return 1;
    case -1:
      printf("\033[0;37m");
      printf("You win. Inconceivable!\n");
      return -1;
  }
}

/*
TicTacToe AI, Có kết nối với server, để sử dụng copy vào file client.c
*/
// int handleTicTacToeAI( char *user) {
//   char *str;
//   int board[9] = {0,0,0,0,0,0,0,0,0};
//   //computer squares are 1, player squares are -1.
//   printf("Computer: O, You: X\n");
//   printf("You want to Play (1)st or (2)nd: ");
//   player=0;
//   scanf("%d",&player);
//   printf("\n");
  
//   for(turn = 0; turn < 9 && win(board) == 0; ++turn) {
//     if( turn != 0 ) showFunGame(user, player);

//     if((turn+player) % 2 == 0){
//       computerMove(board);
//       sprintf(send_msg, "%s#%s", SIGNAL_TICTACTOE_AI, user);
//       if(connectToServer() == 0){    
//         str = strtok(recv_msg, token);
//         if(strcmp(str, SIGNAL_OK) == 0){
//           str = strtok(NULL, token);
//           strcpy(id, str);
//           printf("%s\n", id);
//         }
//       }
//     }
//     else {
//       draw(board);
//       player_move(board);
//     }
//     if( turn == 0 ) write(1,"\E[H\E[2J", 7);
//   }

//   switch(win(board)) {
//     case 0:
//       printf("\033[0;37m");
//       printf("A draw. How droll.\n");
//       return 0;
//     case 1:
//       draw(board);
//       printf("\033[0;37m");
//       printf("You lose.\n");
//       return 1;
//     case -1:
//       printf("\033[0;37m");
//       printf("You win. Inconceivable!\n");
//       return -1;
//   }
// }