#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<windows.h>
#include<time.h>

typedef struct player{   //structure for player
    int x,int y ;
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


int main(){
    return 0;
}
