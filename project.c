#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

#define WIDTH 40
#define HEIGHT 20

typedef struct player {
    int x, y;
} player;

typedef struct bullet {
    int x, y;
    struct bullet *next;
} bullet;

typedef struct enemy {
    int x, y;
    struct enemy *next;
} enemy;

// Game state
player p;
int score = 0;
int highscore = 0;
int enemyspeed = 200; // milliseconds between enemy spawns
int gameover = 0;

bullet *bullets = NULL;
enemy *enemies = NULL;

HANDLE hConsole;
DWORD bytesWritten;

// --- Hide cursor ---
void hidecursor() {
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}

// --- Add bullet ---
void addbullet() {
    bullet *newbullet = (bullet *)malloc(sizeof(bullet));
    newbullet->x = p.x;
    newbullet->y = p.y - 1;
    newbullet->next = bullets;
    bullets = newbullet;
}

// --- Add enemy ---
void addenemy() {
    enemy *newenemy = (enemy *)malloc(sizeof(enemy));
    newenemy->x = rand() % WIDTH;
    newenemy->y = 0;
    newenemy->next = enemies;
    enemies = newenemy;
}

// --- Move bullets ---
void movebullet() {
    bullet *current = bullets, *previous = NULL;
    while (current != NULL) {
        current->y--;
        if (current->y < 0) {
            if (previous == NULL) bullets = current->next;
            else previous->next = current->next;
            bullet *temp = current;
            current = current->next;
            free(temp);
        } else {
            previous = current;
            current = current->next;
        }
    }
}

// --- Move enemies ---
void moveenemy() {
    enemy *current = enemies, *previous = NULL;
    while (current != NULL) {
        current->y++;
        if (current->x == p.x && current->y == p.y) {
            gameover = 1;
            return;
        }
        if (current->y > HEIGHT) {
            if (previous == NULL) enemies = current->next;
            else previous->next = current->next;
            enemy *temp = current;
            current = current->next;
            free(temp);
        } else {
            previous = current;
            current = current->next;
        }
    }
}

// --- Check collisions ---
void checkcollision() {
    bullet *curb = bullets, *prevb = NULL;
    while (curb != NULL) {
        enemy *ecurr = enemies, *eprev = NULL;
        int hit = 0;

        while (ecurr != NULL) {
            if (curb->x == ecurr->x && curb->y == ecurr->y) {
                hit = 1;
                score++;
                if (eprev == NULL) enemies = ecurr->next;
                else eprev->next = ecurr->next;
                free(ecurr);
                break;
            }
            eprev = ecurr;
            ecurr = ecurr->next;
        }

        if (hit) {
            if (prevb == NULL) bullets = curb->next;
            else prevb->next = curb->next;
            bullet *temp = curb;
            curb = curb->next;
            free(temp);
        } else {
            prevb = curb;
            curb = curb->next;
        }
    }
}

// --- Draw frame (no flicker) ---
void draw(int timeLeft) {
    static char buffer[(WIDTH + 3) * (HEIGHT + 7)];
    char *ptr = buffer;

    // Top border
    for (int i = 0; i < WIDTH + 2; i++) *ptr++ = '-';
    *ptr++ = '\n';

    // Draw area
    for (int y = 0; y < HEIGHT; y++) {
        *ptr++ = '|';
        for (int x = 0; x < WIDTH; x++) {
            char ch = ' ';
            if (x == p.x && y == p.y) ch = '^';

            bullet *b = bullets;
            while (b != NULL) {
                if (b->x == x && b->y == y) {
                    ch = '|';
                    break;
                }
                b = b->next;
            }

            enemy *e = enemies;
            while (e != NULL) {
                if (e->x == x && e->y == y) {
                    ch = '*';
                    break;
                }
                e = e->next;
            }

            *ptr++ = ch;
        }
        *ptr++ = '|';
        *ptr++ = '\n';
    }

    // Bottom border
    for (int i = 0; i < WIDTH + 2; i++) *ptr++ = '-';
    *ptr++ = '\n';

    ptr += sprintf(ptr, "Score: %d  High Score: %d  Time Left: %d sec\n", score, highscore, timeLeft);

    DWORD size = ptr - buffer;
    SetConsoleCursorPosition(hConsole, (COORD){0, 0});
    WriteConsoleA(hConsole, buffer, size, &bytesWritten, NULL);
}


void setup() {
    p.x = WIDTH / 2;
    p.y = HEIGHT - 1;
    srand(time(NULL));
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    hidecursor();


    COORD bufferSize = {WIDTH + 4, HEIGHT + 6};
    SetConsoleScreenBufferSize(hConsole, bufferSize);

   
    SMALL_RECT windowSize = {0, 0, WIDTH + 1, HEIGHT + 5};
    SetConsoleWindowInfo(hConsole, TRUE, &windowSize);
}


void game() {
    setup();
    clock_t lastenemy = clock();
    clock_t startTime = clock();
    int gameDuration = 120; // 2 minutes

    while (!gameover) {
        int elapsedTime = (clock() - startTime) / CLOCKS_PER_SEC;
        int timeLeft = gameDuration - elapsedTime;
        if (timeLeft <= 0) break;

        if (_kbhit()) {
            char ch = _getch();
            if (ch == 'a' && p.x > 0) p.x--;
            else if (ch == 'd' && p.x < WIDTH - 1) p.x++;
            else if (ch == ' ') addbullet();
            else if (ch == 'q') gameover = 1;
        }

        movebullet();
        moveenemy();
        checkcollision();

        if ((clock() - lastenemy) * 1000 / CLOCKS_PER_SEC > enemyspeed) {
            addenemy();
            lastenemy = clock();
            if (enemyspeed > 60) enemyspeed -=1;
        }

        draw(timeLeft);
        Sleep(50);
    }

    SetConsoleCursorPosition(hConsole, (COORD){0, HEIGHT + 6});
    printf("\n GAME OVER \n");
    printf("Final Score: %d\n", score);
    if (score > highscore) {
        highscore = score;
        printf(" New High Score: %d \n", highscore);
    }
    printf("\nPress any key to exit...\n");
    _getch();
}

int main() {
    game();
    return 0;
}
