#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

#define WIDTH 40
#define HEIGHT 20

typedef struct player { int x, y; } player;
typedef struct bullet { int x, y; struct bullet *next; } bullet;
typedef struct enemy { int x, y; struct enemy *next; } enemy;

player p;
int score = 0, highscore = 0;
int enemyspeed = 800;
int gameover = 0;

bullet *bullets = NULL;
enemy *enimies = NULL;

HANDLE hConsole;
DWORD bytesWritten;

// === Move cursor ===
void gotoxy(int x, int y) {
    COORD coord = {x, y};
    SetConsoleCursorPosition(hConsole, coord);
}

// === Hide cursor ===
void hidecursor() {
    CONSOLE_CURSOR_INFO info = {100, FALSE};
    SetConsoleCursorInfo(hConsole, &info);
}

// === Add bullet ===
void addbullet() {
    bullet *b = malloc(sizeof(bullet));
    b->x = p.x; b->y = p.y - 1; b->next = bullets;
    bullets = b;
}

// === Add enemy ===
void addenemy() {
    enemy *e = malloc(sizeof(enemy));
    e->x = rand() % WIDTH; e->y = 0; e->next = enimies;
    enimies = e;
}

// === Move bullets ===
void movebullet() {
    bullet *cur = bullets, *prev = NULL;
    while(cur) {
        cur->y--;
        if(cur->y < 0) {
            if(prev) prev->next = cur->next; else bullets = cur->next;
            bullet *tmp = cur; cur = cur->next; free(tmp);
        } else { prev = cur; cur = cur->next; }
    }
}

// === Move enemies ===
void moveenemy() {
    enemy *cur = enimies, *prev = NULL;
    while(cur) {
        cur->y++;
        if(cur->x == p.x && cur->y == p.y) { gameover = 1; return; }
        if(cur->y > HEIGHT) {
            if(prev) prev->next = cur->next; else enimies = cur->next;
            enemy *tmp = cur; cur = cur->next; free(tmp);
        } else { prev = cur; cur = cur->next; }
    }
}

// === Check collisions ===
void checkcollision() {
    bullet *curb = bullets, *prevb = NULL;
    while(curb) {
        enemy *ecurr = enimies, *eprev = NULL;
        int hit = 0;
        while(ecurr) {
            if(curb->x == ecurr->x && curb->y == ecurr->y) {
                hit = 1; score++;
                if(eprev) eprev->next = ecurr->next; else enimies = ecurr->next;
                free(ecurr); break;
            }
            eprev = ecurr; ecurr = ecurr->next;
        }
        if(hit) {
            if(prevb) prevb->next = curb->next; else bullets = curb->next;
            bullet *tmp = curb; curb = curb->next; free(tmp);
        } else { prevb = curb; curb = curb->next; }
    }
}

// === Draw everything using a buffer ===
void draw(int timeLeft) {
    static char buffer[(WIDTH+3)*(HEIGHT+7)];
    char *ptr = buffer;

    for(int i=0;i<WIDTH+2;i++) *ptr++='-'; *ptr++='\n';

    for(int y=0;y<HEIGHT;y++){
        *ptr++='|';
        for(int x=0;x<WIDTH;x++){
            char ch=' ';
            if(x==p.x && y==p.y) ch='^';
            bullet *b = bullets; while(b){ if(b->x==x && b->y==y){ ch='|'; break;} b=b->next; }
            enemy *e = enimies; while(e){ if(e->x==x && e->y==y){ ch='*'; break;} e=e->next; }
            *ptr++ = ch;
        }
        *ptr++='|'; *ptr++='\n';
    }

    for(int i=0;i<WIDTH+2;i++) *ptr++='-'; *ptr++='\n';

    ptr += sprintf(ptr,"Score: %d  High Score: %d  Time Left: %d sec\n",score,highscore,timeLeft);

    DWORD size = ptr-buffer;
    SetConsoleCursorPosition(hConsole,(COORD){0,0});
    WriteConsoleA(hConsole,buffer,size,&bytesWritten,NULL);
}

// === Pause menu ===
int showMenu() {
    gotoxy(0, HEIGHT+4);
    printf("\n=== PAUSED ===\n1. Resume  2. Restart  3. Quit\n");
    char ch;
    while(1){ ch=_getch(); if(ch=='1'||ch=='2'||ch=='3') return ch; }
}

// === Reset game ===
void reset() {
    bullet *b; while(bullets){ b=bullets; bullets=bullets->next; free(b);}
    enemy *e; while(enimies){ e=enimies; enimies=enimies->next; free(e);}
    score=0; gameover=0; p.x=WIDTH/2; p.y=HEIGHT-1;
}

// === Setup ===
void setup() {
    p.x=WIDTH/2; p.y=HEIGHT-1;
    srand(time(NULL));
    hConsole=GetStdHandle(STD_OUTPUT_HANDLE);
    hidecursor();

    COORD bufSize={WIDTH+3,HEIGHT+7};
    SetConsoleScreenBufferSize(hConsole,bufSize);
    SMALL_RECT winSize={0,0,WIDTH+1,HEIGHT+6};
    SetConsoleWindowInfo(hConsole,TRUE,&winSize);

    system("cls");
}

// === Main game loop ===
void game() {
    setup();
    clock_t lastenemy=clock(), startTime=clock();
    int gameDuration=120;

    while(!gameover){
        int elapsedTime=(clock()-startTime)/CLOCKS_PER_SEC;
        int timeLeft=gameDuration-elapsedTime;
        if(timeLeft<=0) break;

        if(_kbhit()){
            char ch=_getch();
            if(ch=='a' && p.x>0) p.x--;
            else if(ch=='d' && p.x<WIDTH-1) p.x++;
            else if(ch==' ') addbullet();
            else if(ch=='q') gameover=1;
            else if(ch=='p'||ch=='P'){
                char choice=showMenu();
                if(choice=='1') continue;
                else if(choice=='2'){ reset(); startTime=clock(); lastenemy=clock(); continue; }
                else if(choice=='3'){ gameover=1; break; }
            }
        }

        movebullet();
        moveenemy();
        checkcollision();

        if((clock()-lastenemy)*1000/CLOCKS_PER_SEC>enemyspeed){ addenemy(); lastenemy=clock(); }

        draw(timeLeft);
        Sleep(50);
    }

    gotoxy(0, HEIGHT+6);
    printf("\nGAME OVER\nFinal Score: %d\n",score);
    if(score>highscore){ highscore=score; printf("New High Score: %d\n",highscore);}
    printf("Press any key to exit...\n"); _getch();
}

// === Main ===
int main(){ 
    game(); 
    return 0; 
}
