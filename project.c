#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<windows.h>
#include<time.h>
#define WIDTH 40
#define HEIGHT 20


typedef struct player{   //structure for player
    int x, y ;
}player;

typedef struct bullet{   //structre created for bullet
    int x, y;
    struct bullet *next;
}bullet;

typedef struct enemy{
    int x, y;
    struct enemy *next;
}enemy;

player p;
int score=0;
int highscore=0;
int enemyspeed=200;
int gameover=0;
enemy *enimies=NULL;
bullet *bullets=NULL;

void gotoxy(int x,int y){
    COORD pos={x,y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void drawborder(){
    for(int i=0;i<WIDTH+2;i++){
        printf("-");
    }
    printf("\n");
}

void addbullet(){
    bullet *newbul=(bullet*)malloc(sizeof(bullet));
    newbul->x=p.x;
    newbul->y=p.y-1;
    newbul->next=bullets;
    bullets=newbul;
}
int main(){
    drawborder();//working
    return 0;
}
