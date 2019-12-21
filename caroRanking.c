#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "caroRanking.h"

char inforCaroRankingOfUser[1000];

caronode* makeNewNodeCaro(userInforCaro user){
  caronode *caronew=(caronode*)malloc(sizeof(caronode));
  caronew->user=user;
  caronew->next=NULL;
  return caronew;
}

void insertCaro(userInforCaro user){
  caronode* caronew=makeNewNodeCaro(user);
  if( caroroot == NULL ) {
    caroroot = caronew;
    carocur = caroroot;
  }else {
    caronew->next=carocur->next;
    carocur->next = caronew;
    carocur = carocur->next;
  }
}

void displayNodeCaro(caronode* p){ // hien thi 1 node
  if (p==NULL){
    printf("Loi con tro NULL\n");
    return; 
  }
  userInforCaro tmp = p->user;
  printf("%-20s%-10d%-10d%-10d%-10.1f\n", tmp.username, tmp.numberOfWin, tmp.numberOfLose, tmp.numberOfDraws, tmp.point);
}

void traversingListCaro(){ // duyet ca list
  caronode* p;
  for ( p = caroroot; p!= NULL; p = p->next )
    displayNodeCaro(p);
}

void to_freeCaro(caronode* caroroot){ // giai phong list
  caronode *to_free;
  to_free=caroroot;
  while(to_free!=NULL){
    caroroot=caroroot->next;
    free(to_free);
    to_free=caroroot;
  }
}

void readFileCaroRanking(){
  FILE *f;
  f = fopen("caroRanking.txt", "r+");
  if(f==NULL){// check file
    printf("Cannot open file caroRanking.txt!!!\n");
    return;
  }
  userInforCaro user;
  while(!feof(f)){
    // dinh dang file: username win lose draw point
    fscanf(f, "%s %d %d %d %f\n", user.username, &user.numberOfWin, &user.numberOfLose, &user.numberOfDraws, &user.point);
    insertCaro(user);
    if(feof(f)) break;
  }
}

void updateFileCaroRanking(){
  FILE *f = fopen("caroRanking.txt", "w");
  caronode* p;
  for ( p = caroroot; p!= NULL; p = p->next ){
    fprintf(f, "%s %d %d %d %.1f\n", p->user.username, p->user.numberOfWin, p->user.numberOfLose, p->user.numberOfDraws, p->user.point);
  }
  fclose(f);
}

caronode* checkUserCaro(char name[100]){// kiem tra user co trong list chua
  caronode* p;
  for ( p = caroroot; p!= NULL; p = p->next ){
    if(strcmp(p->user.username,name)==0){ // =0 la giong nhau
      // printf("%s %d\n", p->user.username, p->user.point);
      return p; // tra ve 1 la user da ton tai
    }
  
  }
  return NULL; // tra ve 0 la user chua ton tai
}

int getInforUserCaro(char name[100]){
  caronode* infor = checkUserCaro(name);
  if( infor != NULL){
    // to-do
    sprintf(inforCaroRankingOfUser,"%s#%d#%d#%d#%f", infor->user.username, infor->user.numberOfWin, infor->user.numberOfLose, infor->user.numberOfDraws, infor->user.point);
    return 1;

  }else{
    // to-do
    return -1;
  }
}

// sort_by_point
void sortCaroRanking(){
  caronode *p, *q;
  userInforCaro tmp;
  for (p = caroroot; p->next != NULL; p = p->next)
    for (q = p->next; q != NULL; q = q->next)
      if (p->user.point < q->user.point)
      {
        tmp = p->user;
        p->user = q->user;
        q->user = tmp;
      }
}

void displayNodeOfCaroRanking(caronode* p, int top, char id[100]){ // hien thi 1 node
  if (p==NULL){
    printf("Loi con tro NULL\n");
    return; 
  }
  caronode* tmpNode = checkUserCaro(id);
  userInforCaro tmp = p->user;
  if( strcmp(tmp.username, tmpNode->user.username ) == 0){
    printf("\033[0;33m%-5d%-20s%-10d%-10d%-10d%-10.1f\033[0;37m\n", top, tmp.username, tmp.numberOfWin, tmp.numberOfLose, tmp.numberOfDraws, tmp.point);
  }else{
    printf("\033[0;37m%-5d%-20s%-10d%-10d%-10d%-10.1f\n", top, tmp.username, tmp.numberOfWin, tmp.numberOfLose, tmp.numberOfDraws, tmp.point);
  }
}

void printfCaroRanking(char id[100]){ // duyet ca list
  sortCaroRanking();
  caronode* p;
  int top = 1;
  for ( p = caroroot; p!= NULL; p = p->next ){
    displayNodeOfCaroRanking(p, top, id);
    top++;
  }
}

void displayNodeOfCaroRankingNotID(caronode* p, int top){ // hien thi 1 node
  if (p==NULL){
    printf("Loi con tro NULL\n");
    return; 
  }
  userInforCaro tmp = p->user;
  printf("\033[0;37m%-5d%-20s%-10d%-10d%-10d%-10.1f\n", top, tmp.username, tmp.numberOfWin, tmp.numberOfLose, tmp.numberOfDraws, tmp.point);
}

void printfCaroRankingNotID(){ // duyet ca list
  sortCaroRanking();
  caronode* p;
  int top = 1;
  for ( p = caroroot; p!= NULL; p = p->next ){
    displayNodeOfCaroRankingNotID(p, top);
    top++;
  }
}