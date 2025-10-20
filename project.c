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
    bullet *newbullet=(bullet*)malloc(sizeof(bullet));
    newbullet->x=p.x;
    newbullet->y=p.y-1;
    newbullet->next=bullets;
    bullets=newbullet;
}

void addenemy(){
    enemy *newenemy=(enemy*)malloc(sizeof(enemy));
    newenemy->x=rand()%width;
    newenemy->y=0;
    newenemy->next=enimies;
    enimies=newenemy;
}

void movebullet(){
    bullet *current=bullets,*previous=NULL;
    while(current!=NULL){
        current->y--;
        if(current->y <0){
            if(previous==NULL){
                bullets=current->next;
            }
            else{
                previous->next=current->next;
            }
            bullet *temp=current;
            current=current->next;
            free(temp);
        }else{
            previous=current;
            current=current->next;
        }
    }
}

void moveenemy(){
    enemy *current=enimies,*previous=NULL;

    while(current!=NULL){
        current->y++;
        if(current->x==p.x && current->y==p.y){
            gameover=1;
            return;
        }
        if(current->y > HEIGHT){
            if(previous==NULL){
                enimies=current->next;
            }
            else{
                previous->next=current->next;
            }
            enemy *temp=current;
            current=current->next;
            free(temp);
        }else{
            previous = current;
            current = current->next;
        }

    }
}

void checkcollision() {
    bullet *curb = bullets, *prevb = NULL;

    while (curb != NULL) {
        enemy *ecurr = enimies, *eprev = NULL;
        int hit = 0;

      
        while (ecurr != NULL) {
            if (curb->x == ecurr->x && curb->y == ecurr->y) {
                hit = 1;
                score++;

               
                if (eprev == NULL)
                    enimies = ecurr->next;
                else
                    eprev->next = ecurr->next;

                free(ecurr);
                break;
            }
            eprev = ecurr;
            ecurr = ecurr->next;
        }

        
        if (hit) {
            if (prevb == NULL)
                bullets = curb->next;
            else
                prevb->next = curb->next;

            bullet *temp = curb;
            curb = curb->next;
            free(temp);
        } 
        else {
            prevb = curb;
            curb = curb->next;
        }
    }
}





int main(){
    drawborder();//working
    return 0;
}
