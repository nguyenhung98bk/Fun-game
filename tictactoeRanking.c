#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "tictactoeRanking.h"

char inforTTTRankingOfUser[1000];

node* makeNewNode(user_infor user){
  node *new=(node*)malloc(sizeof(node));
  new->user=user;
  new->next=NULL;
  return new;
}

void insert(user_infor user){
  node* new=makeNewNode(user);
  if( TTTroot == NULL ) {
    TTTroot = new;
    cur = TTTroot;
  }else {
    new->next=cur->next;
    cur->next = new;
    cur = cur->next;
  }
}

void displayNode(node* p){ // hien thi 1 node
  if (p==NULL){
    printf("Loi con tro NULL\n");
    return; 
  }
  user_infor tmp = p->user;
  printf("%-20s%-10d%-10d%-10d%-10.1f\n", tmp.username, tmp.numberOfWin, tmp.numberOfLose, tmp.numberOfDraws, tmp.point);
}

void traversingList(){ // duyet ca list
  node* p;
  for ( p = TTTroot; p!= NULL; p = p->next )
    displayNode(p);
}

void to_free(node* TTTroot){ // giai phong list
  node *to_free;
  to_free=TTTroot;
  while(to_free!=NULL){
    TTTroot=TTTroot->next;
    free(to_free);
    to_free=TTTroot;
  }
}

void readFileTTTRanking(){
  FILE *f;
  f = fopen("tictactoeRanking.txt", "r+");
  if(f==NULL){// check file
    printf("Cannot open file tictactoeRanking.txt!!!\n");
    return;
  }
  user_infor user;
  while(!feof(f)){
    // dinh dang file: username win lose draw point
    fscanf(f, "%s %d %d %d %f\n", user.username, &user.numberOfWin, &user.numberOfLose, &user.numberOfDraws, &user.point);
    insert(user);
    if(feof(f)) break;
  }
}

void updateFileTTTRanking(){
  FILE *f = fopen("tictactoeRanking.txt", "w");
  node* p;
  for ( p = TTTroot; p!= NULL; p = p->next ){
    fprintf(f, "%s %d %d %d %.1f\n", p->user.username, p->user.numberOfWin, p->user.numberOfLose, p->user.numberOfDraws, p->user.point);
  }
  fclose(f);
}

node* checkUser(char name[100]){// kiem tra user co trong list chua
  node* p;
  for ( p = TTTroot; p!= NULL; p = p->next ){
    if(strcmp(p->user.username,name)==0){ // =0 la giong nhau
      // printf("%s %d\n", p->user.username, p->user.point);
      return p; // tra ve 1 la user da ton tai
    }
  
  }
  return NULL; // tra ve 0 la user chua ton tai
}

int getInforUser(char name[100]){
  node* infor = checkUser(name);
  if( infor != NULL){
    // to-do
    sprintf(inforTTTRankingOfUser,"%s#%d#%d#%d#%f", infor->user.username, infor->user.numberOfWin, infor->user.numberOfLose, infor->user.numberOfDraws, infor->user.point);
    return 1;

  }else{
    // to-do
    return -1;
  }
}

// sort_by
void sortTTTRanking(){
  node *p, *q;
  user_infor tmp;
  for (p = TTTroot; p->next != NULL; p = p->next)
    for (q = p->next; q != NULL; q = q->next)
      if (p->user.point < q->user.point)
      {
        tmp = p->user;
        p->user = q->user;
        q->user = tmp;
      }
}

void displayNodeOfTTTRanking(node* p, int top, char id[100]){ // hien thi 1 node
  if (p==NULL){
    printf("Loi con tro NULL\n");
    return; 
  }
  node* tmpNode = checkUser(id);
  user_infor tmp = p->user;
  if( strcmp(tmp.username, tmpNode->user.username ) == 0){
    printf("\033[0;33m%-5d%-20s%-10d%-10d%-10d%-10.1f\033[0;37m\n", top, tmp.username, tmp.numberOfWin, tmp.numberOfLose, tmp.numberOfDraws, tmp.point);
  }else{
    printf("\033[0;37m%-5d%-20s%-10d%-10d%-10d%-10.1f\n", top, tmp.username, tmp.numberOfWin, tmp.numberOfLose, tmp.numberOfDraws, tmp.point);
  }
}

void printfTTTRanking(char id[100]){ // duyet ca list
  sortTTTRanking();
  node* p;
  int top = 1;
  for ( p = TTTroot; p!= NULL; p = p->next ){
    displayNodeOfTTTRanking(p, top, id);
    top++;
  }
}

void displayNodeOfTTTRankingNotID(node* p, int top){ // hien thi 1 node
  if (p==NULL){
    printf("Loi con tro NULL\n");
    return; 
  }
  user_infor tmp = p->user;
  printf("\033[0;37m%-5d%-20s%-10d%-10d%-10d%-10.1f\n", top, tmp.username, tmp.numberOfWin, tmp.numberOfLose, tmp.numberOfDraws, tmp.point);
}

void printfTTTRankingNotID(){ // duyet ca list
  sortTTTRanking();
  node* p;
  int top = 1;
  for ( p = TTTroot; p!= NULL; p = p->next ){
    displayNodeOfTTTRankingNotID(p, top);
    top++;
  }
}