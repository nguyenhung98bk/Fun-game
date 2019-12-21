#include <stdio.h>
#include <stdlib.h>

typedef struct elementtype{
  char username[50]; // Khai bao username
  int numberOfWin;
  int numberOfLose;
  int numberOfDraws;
  float point;
}user_infor;

struct node{
  user_infor user;
  struct node *next;
};
typedef struct node node;
node *TTTroot,*cur,*new;

node* makeNewNode(user_infor user);

void insert(user_infor user);

void displayNode(node* p);

void traversingList();

void to_free(node* TTTroot);

void readFileTTTRanking();

void updateFileTTTRanking();

node* checkUser(char name[100]);

int getInforUser(char name[100]);

void sortTTTRanking();

void displayNodeOfTTTRanking(node* p, int top, char id[100]);

void printfTTTRanking(char id[100]);

void displayNodeOfTTTRankingNotID(node* p, int top);

void printfTTTRankingNotID();