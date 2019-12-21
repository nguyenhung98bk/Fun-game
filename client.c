/*
Written by NoHope Team
Members:
-----Vu Duc Nguyen-----(L)
-----Nguyen Viet Hung-----
-----Phung Thi Trang------
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
/*
Library of socket
*/
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
/*
https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.1.0/com.ibm.zos.v2r1.bpxbd00/rttcga.htm
https://www.daemon-systems.org/man/tcgetattr.3.html
*/
#include <termios.h>
#include <assert.h>
#include <signal.h>
#include <errno.h>

#include "linklist.h"
#include "checkinput.h"
#include "tic-tac-toe.h"
#include "tictactoeRanking.h"
#include "caroRanking.h"
#include "clientHelper.h"

#define BUFF_SIZE 1024
int VIEWSIZE = 15;

// fungame
#define STATUS_START_MENU 0
#define STATUS_FUNGAME_MENU 1
#define STATUS_HANDLE_GAME 2
#define SIGNAL_CHECKLOGIN "SIGNAL_CHECKLOGIN"
#define SIGNAL_CREATEUSER "SIGNAL_CREATEUSER"
#define SIGNAL_OK "SIGNAL_OK"
#define SIGNAL_ERROR "SIGNAL_ERROR" 
#define SIGNAL_CLOSE "SIGNAL_CLOSE"

// caro game
#define SIGNAL_CARO_NEWGAME "SIGNAL_CARO_NEWGAME"
#define SIGNAL_CARO_ABORTGAME "SIGNAL_CARO_ABORTGAME"
#define SIGNAL_CARO_TURN "SIGNAL_CARO_TURN"
#define SIGNAL_CARO_WIN "SIGNAL_CARO_WIN"
#define SIGNAL_CARO_LOST "SIGNAL_CARO_LOST"
#define SIGNAL_CARO_VIEWLOG "SIGNAL_CARO_VIEWLOG"
#define SIGNAL_LOGLINE "SIGNAL_LOGLINE"
// show info
#define CARO_INFO_QUIT "Quit caro game (y or n)?"
#define CARO_INFO_WIN "You Win. Do you want to view log (y or n)?"
#define CARO_INFO_LOST "You Lose. Do you want to view log (y or n)?"

// caro ranking
#define SIGNAL_CARO_RANKING "SIGNAL_CARO_RANKING"

// tictactoe game
#define SIGNAL_TICTACTOE "SIGNAL_TICTACTOE"
#define SIGNAL_TTT_RESULT "SIGNAL_TTT_RESULT"
// #define SIGNAL_TICTACTOE_AI "SIGNAL_TICTACTOE_AI"

// tictactoe ranking
#define SIGNAL_TTT_RANKING "SIGNAL_TTT_RANKING"

// client connect to server
struct sockaddr_in server_addr;
int PORT, sock, recieved, isCommunicating;
char* serverAddress;
char send_msg[BUFF_SIZE], recv_msg[BUFF_SIZE];

// client variable
int status; // status of fungame
char choice;
char error[100], user[100], id[30];
char token[] = "#";

// draw table
char* table;
int size, playerTurn, col, row;
int viewC = 0, viewR = 0;

// trang thai choi caro hay tic-tac-toe, 1 la caro, 2 la tic-tac-toe
int checkCaroOrTicTacToe;

// result of TicTacToe
int resultTTT;

/*
Kết nối với server
*/
int connectToServer(){
  isCommunicating = 1; // tao ket noi
  int errorConnect;

  //Step 1: Construct socket
  if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1){
    strcpy(error,"Error Socket!!!");
    return -1;
  }
  
  //Step 2: Specify server address
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = inet_addr(serverAddress);

  //Step 3: Request to connect server
  if( connect(sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) == -1){
    errorConnect = errno;
    sprintf(error,"Error! Can not connect to server! %s",strerror(errorConnect));		
    return -1;
  }

  //Step 4: Communicate with server
  send(sock, send_msg, strlen(send_msg), 0);
  recieved = recv(sock, recv_msg, BUFF_SIZE, 0);
  recv_msg[recieved] = '\0';
  
  strcpy(send_msg, SIGNAL_CLOSE);
  send(sock, send_msg, strlen(send_msg), 0);
  close(sock);
  isCommunicating = 0; // ngat ket noi
  return 0;
}

/*
Hiển thị phần login
*/
int menuSignin(){
  error[0] = '\0';
  char pass[100], *str;
  while(1){
    clearScreen();
    printf("----------FUNGAME----------\n");
    printf("\tSign in\n");
    printf("---------------------------\n");
    if(error[0] != '\0'){
      printf("Error: %s!\n", error);
      printf("Do you want to try again? (y or n)\n");
      printf("Your choice: ");
      choice = getchar();
      while(getchar() != '\n');
      if(choice=='n' || choice=='N'){
      	error[0] = '\0';
      	return -1;
      }
      else if(choice !='y' && choice !='Y'){
        continue;
      }
      else{
      	error[0] = '\0';
      	continue;
      }
    }
    printf("Username: ");
    // gets(user);
    fgets(user, BUFF_SIZE, stdin);
    user[strlen(user)-1] = '\0';

    printf("Password: ");
    getPassword(pass);

    //check username and password
    sprintf(send_msg, "%s#%s#%s", SIGNAL_CHECKLOGIN, user, pass);
    if(connectToServer() == 0){    
      str = strtok(recv_msg, token);
      if(strcmp(str, SIGNAL_OK) == 0)
        break;    
      else if(strcmp(str, SIGNAL_ERROR) == 0){
      	str = strtok(NULL, token);
      	strcpy(error, str);
      }
    }    
  }  
  return 0;
}

/*
Hiển thị phần đăng kí
*/
int menuRegister(){
  error[0] = '\0';
  char pass[100], comfirmPass[100], *str;
  while(1){
    clearScreen();
    printf("----------FUNGAME----------\n");
    printf("\tRegister\n");
    printf("---------------------------\n");
    if(error[0] != '\0'){
      printf("Error: %s!\n", error);
      printf("Do you want to try again? (y or n)\n");
      printf("Your choice: ");
      choice = getchar();
      while(getchar() != '\n');
      if(choice =='n' || choice =='N'){
      	error[0] = '\0';
      	return -1;
      }
      else if(choice !='y' && choice !='Y')
        continue;
      else{
      	error[0] = '\0';
      	continue;
      }
    }
    printf("Username: ");
    fgets(user, BUFF_SIZE, stdin);
    user[strlen(user)-1] = '\0';

    printf("Password: ");
    getPassword(pass);
    printf("\nConfirm password: ");
    getPassword(comfirmPass);
    if(strcmp(pass, comfirmPass) == 0){
      // register new account
      sprintf(send_msg, "%s#%s#%s", SIGNAL_CREATEUSER, user, pass);
      if(connectToServer() == 0){    
      	str = strtok(recv_msg, token);
      	if(strcmp(str, SIGNAL_OK) == 0)
      	  break;
      	else if(strcmp(str, SIGNAL_ERROR) == 0){
      	  str = strtok(NULL, token);
      	  strcpy(error, str);
      	}
      }
    }
    else{
      strcpy(error, "Password does not match");
    }
  }  
  return 0;
}

/*
Hiển thị chọn login, create
*/
int menuStart(){  
  error[0] = '\0';  
  while(1){
    clearScreen();
    if(error[0] != '\0'){
      printf("Error: %s!\n", error );
      error[0] = '\0';
    }
    printf("----------FUNGAME----------\n");
    printf("\t1.Sign in\n");
    printf("\t2.Register\n");
    printf("\t3.Exit\n");
    printf("---------------------------\n");
    printf("Your choice: ");
    scanf("%c", &choice);
    while(getchar() != '\n');
    if(choice == '1'){
      if(menuSignin() == 0)
        break;
    }
    else if(choice == '2'){
      if(menuRegister() == 0)
        break;
    }
    else if(choice == '3') return -1;
    else sprintf(error,"No option %c", choice);
  }

  return 0;
}

/*
Hiển thị phần game caro, chọn size để chơi
*/
int menuCaroGame(){
  error[0] = '\0';
  char* str;
  while(1){
    clearScreen();
    printf("----------FUNGAME----------\n");
    printf("\033[0;33m\tUsername: %s \033[0;37m\n", user);
    printf("\tNEW CARO GAME\n");
    printf("---------------------------\n");
    if(error[0] != '\0'){
      printf("Error: %s!\n", error);
      printf("Do you want to try again? (y or n)\n");
      printf("Your choice: ");
      choice = getchar();
      while(getchar() != '\n');
      if(choice =='n' || choice =='N'){
      	error[0] = '\0';
      	return -1;
      }
      else if(choice !='y' && choice !='Y')
        continue;
      else{
      	error[0] = '\0';
      	continue;
      }
    }   
    printf("Enter size of table (from 15 to 25): ");
    scanf("%d", &size);
    VIEWSIZE = size;
    while(getchar() != '\n');
    if(size >= 15 &&  size <= 25){
      sprintf(send_msg, "%s#%d#%s", SIGNAL_CARO_NEWGAME, size, user);
      if(connectToServer() == 0){    
      	str = strtok(recv_msg, token);
      	if(strcmp(str, SIGNAL_OK) == 0){
      	  str = strtok(NULL, token);
      	  strcpy(id, str);
      	  //init table
      	  table = malloc(size * size);
      	  for(int i = 0; i < size * size; i++){
      	    table[i] = ' ';
          }
      	  break;    
      	}
      	else if(strcmp(str, SIGNAL_ERROR) == 0){
      	  str = strtok(NULL, token);
      	  strcpy(error, str);
      	}
      }
    }
    else{
      sprintf(error, "Size %d | size error!!!", size);
    }
  }  
  return 0;
}

/*
Hiển thị phần tic-tac-toe
*/
int handleTicTacToeGame(){
  char* str;
  error[0] = '\0';
  int i;
  while(1){
    clearScreen();
    printf("----------FUNGAME----------\n");
    printf("\033[0;33m\tUsername: %s \033[0;37m\n", user);
    printf("\tNEW TicTacToe GAME\n");
    printf("---------------------------\n");
    if(error[0] != '\0'){
      printf("Error: %s!\n", error);
      printf("Do you want to try again? (y or n)\n");
      printf("Your choice: ");
      choice = getchar();
      while(getchar() != '\n');
      if(choice == 'n' || choice == 'N'){
        error[0] = '\0';
        return -1;
      }
      else if(choice != 'y' && choice != 'Y')
        continue;
      else{
        error[0] = '\0';
        continue;
      }
    }

    sprintf(send_msg, "%s#%s", SIGNAL_TICTACTOE, user);
    if(connectToServer() == 0){
      str = strtok(recv_msg, token);
      if(strcmp(str, SIGNAL_OK) == 0){
        str = strtok(NULL, token);
        strcpy(id, str);

        resultTTT = handleTicTacToe(user); // 0 hòa, 1 thua, -1 thắng
        sprintf(send_msg, "%s#%s#%d", SIGNAL_TTT_RESULT, id, resultTTT);
        if(connectToServer() == 0){    
          str = strtok(recv_msg, token);
          if(strcmp(str, SIGNAL_OK) == 0){
            str = strtok(NULL, token);
            strcpy(id, str);
            printf("Do you want to play again? (y or n)\n");
            printf("Your choice: ");
            while(getchar() != '\n');
            choice = getchar();
            while(getchar() != '\n');
            
            if(choice == 'n' || choice == 'N'){
              return -1;
            }
            else if(choice != 'y' && choice != 'Y'){
              continue;
            }
            else{
              continue;
            }
          }
          else if(strcmp(str, SIGNAL_ERROR) == 0){
            str = strtok(NULL, token);
            strcpy(error, str);
          }
        }
      }
      else if(strcmp(str, SIGNAL_ERROR) == 0){ // error connect to server
        str = strtok(NULL, token);
        strcpy(error, str);
      }
    }
  }
  return 0;
}

int handleCaroRanking(){
  char* str;
  error[0] = '\0';

  clearScreen();
  printf("----------FUNGAME----------\n");
  printf("\033[0;33m\tUsername: %s \033[0;37m\n", user);
  printf("--------Caro Ranking--------\n");
  printf("Your information: \n");

  sprintf(send_msg, "%s#%s", SIGNAL_CARO_RANKING, user);
  if(connectToServer() == 0){
    str = strtok(recv_msg, token);
    if(strcmp(str, SIGNAL_OK) == 0){
      str = strtok(NULL, token);
      strcpy(id, str);
      int numberOfWin, numberOfLose, numberOfDraws;
      float point;

      str = strtok(NULL, token);
      numberOfWin = atoi(str);

      if(numberOfWin == -1){
        printf("You haven't played yet Caro. You are not in the ranking.\n");
      } else{
        str = strtok(NULL, token); numberOfLose = atoi(str);
        str = strtok(NULL, token); numberOfDraws = atoi(str);
        str = strtok(NULL, token); point = atof(str);
        printf("\t%-20s: %s\n","Username-ID", id);
        printf("\t%-20s: %d\n","Number Of Wins", numberOfWin);
        printf("\t%-20s: %d\n","Number Of Losses", numberOfLose);
        printf("\t%-20s: %d\n","Number Of Draws", numberOfDraws);
        printf("\t%-20s: %.1f\n","Point", point);
      }
      printf("------------------------------\n");
      printf("Caro Ranking\n");
      printf("------------------------------------------------------------\n");
      readFileCaroRanking();
      printf("%-5s%-20s%-10s%-10s%-10s%-10s\n", "TOP", "ID", "Win", "Lose", "Draw", "Point");
      if(numberOfWin == -1) printfCaroRankingNotID();
      else printfCaroRanking(id);
      caroroot = NULL; carocur = NULL; caronew = NULL; 
    }
  }
  else {
    printf("Error! Cant connect to server!\n");
  }
  printf("Press 'q' to quit: ");
  choice = getchar();
  while(getchar() != '\n');
  if(choice == 'q' || choice == 'Q') return 1;
  else return 1;
}

int handleTicTacToeRanking(){
  char* str;
  error[0] = '\0';

  clearScreen();
  printf("----------FUNGAME----------\n");
  printf("\033[0;33m\tUsername: %s \033[0;37m\n", user);
  printf("------TicTacToe Ranking------\n");
  printf("Your information: \n");

  sprintf(send_msg, "%s#%s", SIGNAL_TTT_RANKING, user);
  if(connectToServer() == 0){
    str = strtok(recv_msg, token);
    if(strcmp(str, SIGNAL_OK) == 0){
      str = strtok(NULL, token);
      strcpy(id, str);
      int numberOfWin, numberOfLose, numberOfDraws;
      float point;

      str = strtok(NULL, token);
      numberOfWin = atoi(str);

      if(numberOfWin == -1){
        printf("You haven't played yet tic-tac-toe. You are not in the ranking.\n");
      } else{
        str = strtok(NULL, token); numberOfLose = atoi(str);
        str = strtok(NULL, token); numberOfDraws = atoi(str);
        str = strtok(NULL, token); point = atof(str);
        printf("\t%-20s: %s\n","Username-ID", id);
        printf("\t%-20s: %d\n","Number Of Wins", numberOfWin);
        printf("\t%-20s: %d\n","Number Of Losses", numberOfLose);
        printf("\t%-20s: %d\n","Number Of Draws", numberOfDraws);
        printf("\t%-20s: %.1f\n","Point", point);
      }
      printf("------------------------------\n");
      printf("TicTacToe Ranking\n");
      printf("------------------------------------------------------------\n");
      readFileTTTRanking();
      printf("%-5s%-20s%-10s%-10s%-10s%-10s\n", "TOP", "ID", "Win", "Lose", "Draw", "Point");
      if(numberOfWin == -1) printfTTTRankingNotID();
      else printfTTTRanking(id);
      TTTroot = NULL; cur = NULL; new = NULL; 
    }
  }
  else {
    printf("Error! Cant connect to server!\n");
  }
  printf("Press 'q' to quit: ");
  choice = getchar();
  while(getchar() != '\n');
  if(choice == 'q' || choice == 'Q') return 1;
  else return 1;
}

/*
Hiển thị phần xử lí game, chọn game caro hay tic-tok-toe
*/
int menuFunGame(){
  error[0] = '\0';  
  while(1){
    clearScreen();
    if(error[0] != '\0'){
      printf("Error: %s!\n", error);
      error[0] = '\0';
    }
    printf("----------FUNGAME----------\n");
    printf("\033[0;33m\tWelcome %s \033[0;37m\n", user);
    printf("\t1.Caro game\n");
    printf("\t2.TicTacToe game\n");
    printf("\t3.Caro ranking\n");
    printf("\t4.TicTacToe ranking\n");
    printf("\t5.Sign out\n");
    printf("---------------------------\n");
    printf("Your choice: ");
    scanf("%c", &choice);
    while(getchar() != '\n');
    if(choice == '1'){
      checkCaroOrTicTacToe = 1;
      if(menuCaroGame() == 0) break;
    }
    else if(choice == '2'){
      checkCaroOrTicTacToe = 2;
      if(handleTicTacToeGame() == 0) break;
    }
    else if(choice == '3'){
      checkCaroOrTicTacToe = 3;
      break;
    }
    else if(choice == '4'){
      checkCaroOrTicTacToe = 4;
      break;
    }
    else if(choice == '5'){
      return -1;
    }
    else {
      sprintf(error,"%c Not an optional", choice);      
    }
  }
  return 0;
}

/*
View log
*/
int viewLog(){  
  char usernameLog[100], *str;

  sprintf(send_msg, "%s#%s", SIGNAL_CARO_VIEWLOG, id);
  if(connectToServer() == 0){    
    // get file name
    int j = 0;
    for(int i = 15; i < strlen(recv_msg); i++){
      usernameLog[j] = recv_msg[i];
      if(recv_msg[i] == '\0') break;
      j++;
    }
    usernameLog[j] = '\0';

    str = strtok(recv_msg, token);
    
    if(strcmp(str, SIGNAL_LOGLINE) == 0){
      str = strtok(NULL, token);
      printf("Username: %s ", str);
      
      str = strtok(NULL, token);
      printf("- SessionID: %s\n", str);
      str = strtok(NULL, token); // address
      // printf(" - Address: %s\n", str);
      str = strtok(NULL, token);
      printf("Day: %s ", str);
      str = strtok(NULL, token);
      str[strlen(str) - 4] = '\0';
      printf("- Time: %s\n", str);
    }
    else if(strcmp(str, SIGNAL_ERROR) == 0){
      strcpy(send_msg, SIGNAL_CLOSE);
      send(sock, send_msg, strlen(send_msg), 0);
      close(sock);
      isCommunicating = 0;
      str = strtok(NULL, token);
      strcpy(error, str);
      return -1;
    }
  }
  printLog(usernameLog); // in log
  printf("Press 'q' to quit: ");
  choice = getchar();
  while(getchar() != '\n');
  if(choice == 'q' || choice == 'Q') return 0;
  else return 0;
}

/*
Vẽ bảng chơi caro
*/
void drawTable(){
  int i, j, k;
  
  printf("   ");
  for(i = 0; i < VIEWSIZE; i++){
    if(i < 10) printf("%d  ", i);
    else printf("%d ", i);
  }
  printf("\n");

  for(i = 0; i < VIEWSIZE; i++){
    if(i < 10) printf("%d  ", i);
    else printf("%d ", i);

    for(j = 0; j < VIEWSIZE; j++){
      if(i == row && j == col){
        printf("\033[0;37m[\033[0;37m%c\033[0;37m]", table[i * size + j]); // white
      }
      else{
        if(table[i * size + j] == 'x')
          printf("\033[0;36m[\033[0;33m%c\033[0;36m]", table[i * size + j]); // yellow
        else if (table[i * size + j] == 'o')
          printf("\033[0;36m[\033[0;31m%c\033[0;36m]", table[i * size + j]); // red
        else
          printf("\033[0;36m[\033[0;35m%c\033[0;36m]", table[i * size + j]); // bue
      }
    }    
    printf("\n");
  }
}

/*
Xử lí, điều khiển quân bằng w,a,s,d, ấn q thì thoát
*/
int handleCaroGame(){
  char c, info[100], *str;
  setCustomTerminal();
  error[0] = '\0';  
  info[0] = '\0';

  playerTurn = 1;
  col = size / 2;
  row = size / 2;
  while(1){
    clearScreen();
    printf("\033[0;37m----------CARO GAME----------\n");
    printf("\033[0;33m\tUsername: %s - GameID = %s\033[0;37m\n", user, id);    
    printf("\tPress w,a,s,d to move \n");
    printf("\tPress enter to select\n");
    printf("\tPress 'q' to quit\n");
    printf("-----------------------------\n");   
    drawTable();
    if(playerTurn){
      if(error[0] != '\0'){
      	setDefaultTerminal();

      	printf("Error: %s!\n", error);
      	printf("Do you want to try again? (y or n)\n");
      	printf("Your choice: ");
      	choice = getchar();
      	while(getchar() != '\n');
      	if(choice == 'n' || choice == 'N'){
      	  error[0] = '\0';	  
      	  return -1;
      	}
      	else if(choice == 'y' || choice == 'Y'){
      	  error[0] = '\0';	  
      	  if(info[0] == '\0')
      	    playerTurn = 0;
      	  else if(strcmp(info, CARO_INFO_WIN) == 0 || strcmp(info, CARO_INFO_LOST) == 0){
      	    if(choice == 'y' || choice == 'Y'){
      	      if(viewLog() == 0){
      		sprintf(send_msg, "%s#%s#%s",SIGNAL_CARO_ABORTGAME, id, user);
      		connectToServer();
      		return -1;
      	      }
      	    }
      	  }
      	}
      	setCustomTerminal();
      }
      else if(info[0] != '\0'){
      	setDefaultTerminal();

      	printf("\033[0;37m%s\n", info);
      	printf("Your choice: ");
      	choice = getchar();
      	while(getchar() != '\n');
      	if(choice == 'n' || choice == 'N'){
      	  if(strcmp(info, CARO_INFO_QUIT) == 0)
      	    info[0] = '\0';
      	  else if(strcmp(info, CARO_INFO_WIN) == 0 || strcmp(info, CARO_INFO_LOST) == 0){
      	    sprintf(send_msg, "%s#%s#%s",SIGNAL_CARO_ABORTGAME, id, user);
      	    connectToServer();
      	    return -1;
      	  }
      	}
      	else if(choice == 'y' || choice == 'Y'){
      	  if(strcmp(info, CARO_INFO_QUIT) == 0){
      	    sprintf(send_msg, "%s#%s#%s",SIGNAL_CARO_ABORTGAME, id, user);
      	    connectToServer();
      	    return -1;	
      	  }
      	  else if(strcmp(info, CARO_INFO_WIN) == 0 || strcmp(info, CARO_INFO_LOST) == 0){
      	    if(viewLog() == 0){
      	      sprintf(send_msg, "%s#%s#%s",SIGNAL_CARO_ABORTGAME, id, user);
      	      connectToServer();
      	      return -1;
      	    }
      	  }
      	}
      	setCustomTerminal();  
      }
      else{
      	c = getchar();
      	if(c == 'w' && row > 0) row--;
      	else if(c == 's' && row < size - 1) row++;
      	else if(c == 'd' && col < size - 1) col++;
      	else if(c == 'a' && col > 0) col--;
      	else if(c == '\n' && table[row * size + col] == ' '){
      	  table[row * size + col] = 'x';
      	  playerTurn = 0;	  	  
      	}
      	else if(c == 'q') strcpy(info, CARO_INFO_QUIT);
      }
    }
    else {
      sprintf(send_msg, "%s#%s#%s#%d#%d", SIGNAL_CARO_TURN, id, user, col, row);
      if(connectToServer() == 0){
      	str = strtok(recv_msg, token);
      	if(strcmp(str, SIGNAL_CARO_TURN) == 0){
      	  str = strtok(NULL, token);
      	  col = atoi(str);
      	  str = strtok(NULL, token);
      	  row = atoi(str);
      	  table[row * size + col] = 'o';
      	}
      	else if(strcmp(str, SIGNAL_CARO_WIN) == 0)
      	  strcpy(info, CARO_INFO_WIN);
      	else if(strcmp(str, SIGNAL_CARO_LOST) == 0){
      	  str = strtok(NULL, token);
      	  col = atoi(str);
      	  str = strtok(NULL, token);
      	  row = atoi(str);
      	  table[row * size + col] = 'o';
      	  strcpy(info, CARO_INFO_LOST);
      	}
      	else if(strcmp(str, SIGNAL_ERROR) == 0){
      	  str = strtok(NULL, token);
      	  strcpy(error, str);
      	}
      }
      playerTurn = 1;
    }
  }
  setDefaultTerminal();
  return 0;
}

int main(int argc, char* argv[]){     
  int err;
  if(argc != 3){
    printf("Syntax Error.\n");
    printf("Syntax: ./client IPAddress PortNumber\n");
    return 0;
  }
  if(check_IP(argv[1]) == 0){
    printf("IP address invalid\n");
    return 0;
  }
  if(check_port(argv[2]) == 0){
    printf("Port invalid\n");
    return 0;
  }
  serverAddress = argv[1];
  PORT = atoi(argv[2]);
  
  status = STATUS_START_MENU;  
  while(1){
    if(status == STATUS_START_MENU){
      if(menuStart() == -1)
        break;
      else
        status = STATUS_FUNGAME_MENU;
    }
    else if(status == STATUS_FUNGAME_MENU){
      if(menuFunGame() == - 1)
        status = STATUS_START_MENU;	
      else
        status = STATUS_HANDLE_GAME;
    }
    else if(status == STATUS_HANDLE_GAME){
      if(checkCaroOrTicTacToe == 1){
        if(handleCaroGame() == - 1){
          free(table);
          table = NULL;
          status = STATUS_FUNGAME_MENU;     
        }
      }else if(checkCaroOrTicTacToe == 2){
        if( handleTicTacToeGame() == -1) {
          status = STATUS_FUNGAME_MENU;
        }
      }
      else if(checkCaroOrTicTacToe == 3){
        handleCaroRanking();
        status = STATUS_FUNGAME_MENU;
      }
      else if(checkCaroOrTicTacToe == 4){
        handleTicTacToeRanking();
        status = STATUS_FUNGAME_MENU;
      }
    }
  }
  return 0;
}