#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <tchar.h>
#include <time.h>
bool gamePaused = false;
bool congratulationsShown = false;
int lives = 3;

struct Board {
    int x, y;
    int width;
};

struct Heart {
    int x, y;
    bool isActive;
};

void drawHeart(int x, int y, int size) {
    setfillcolor(RED);
    int heart[8] = {
        x, y + size / 4,
        x + size / 2, y,
        x + size, y + size / 4,
        x + size / 2, y + size,
    };
    fillpoly(4, heart);
}

void gameLoop();

void startGame() {
    gameLoop();
}

void gameLoop() {
    int w = 800, h = 700;
    int bh = 20;
    int speed = 5;
    int circleX, circleY;
    int jumpHeight = 100;
    int gravity = 5;
    bool isJumping = false;
    int jumpStartY;
    int i;

    int ballRadius = 20;
    int totalBoardsPassed = 0;
    lives = 3;

    srand((unsigned int)time(NULL));

    Board board[6];
    Heart hearts[6];

    for (i = 0; i < 6; i++) {
        board[i].width = rand() % 150 + 70;
        board[i].x = rand() % (w - board[i].width);
        board[i].y = 590 - (i * 100);
        hearts[i].isActive = false;
    }

    for (i = 0; i < 6; i++) {
        if (rand() % 2 == 0) {
            hearts[i].isActive = true;
            hearts[i].x = board[i].x + board[i].width / 2;
            hearts[i].y = board[i].y - 30;
        }
    }

    circleX = board[0].x + board[0].width / 2;
    circleY = board[0].y - ballRadius;

    initgraph(w, h);

    BeginBatchDraw();
    while (true) {
        cleardevice();

        setfillcolor(RGB(255, 165, 0));
        for (int thornX = 0; thornX < w; thornX += 80) {
            line(thornX, 50, thornX + 40, 0);
            line(thornX + 40, 0, thornX + 80, 50);
        }

        for (i = 0; i < 6; i++) {
            setfillcolor(YELLOW);
            fillrectangle(board[i].x, board[i].y, board[i].x + board[i].width, board[i].y + bh);
            board[i].y -= speed;

            if (board[i].y + bh < circleY && board[i].y + bh + speed >= circleY) {
                totalBoardsPassed++;
            }

            if (hearts[i].isActive) {
                hearts[i].y -= speed;
                if (hearts[i].y < 0) {
                    hearts[i].isActive = false;
                }
            }

            if (board[i].y < 0) {
                board[i].y = h - bh;
                board[i].width = rand() % 150 + 70;
                if (rand() % 2 == 0) {
                    hearts[i].isActive = true;
                    hearts[i].x = board[i].x + board[i].width / 2;
                    hearts[i].y = board[i].y - 30;
                }
                else {
                    hearts[i].isActive = false;
                }
            }

            if (hearts[i].isActive) {
                drawHeart(hearts[i].x, hearts[i].y, 20);
            }
        }

        setfillcolor(WHITE);
        fillcircle(circleX, circleY, ballRadius);

        if (isJumping) {
            if (circleY > jumpStartY - jumpHeight) {
                circleY -= gravity;
            }
            else {
                isJumping = false;
            }
        }
        else {
            bool onBoard = false;
            for (i = 0; i < 6; i++) {
                if (circleX >= board[i].x && circleX <= board[i].x + board[i].width &&
                    circleY + ballRadius >= board[i].y && circleY + ballRadius <= board[i].y + bh) {
                    onBoard = true;
                    circleY = board[i].y - ballRadius;
                    if (hearts[i].isActive && circleX >= hearts[i].x - 20 && circleX <= hearts[i].x + 20 &&
                        circleY + ballRadius >= hearts[i].y) {
                        lives++;
                        hearts[i].isActive = false;
                        TCHAR lifeMessage[50];
                        _stprintf_s(lifeMessage, sizeof(lifeMessage) / sizeof(TCHAR), _T("Lives: %d"), lives);
                        settextcolor(GREEN);
                        outtextxy(10, 50, lifeMessage);
                    }
                    break;
                }
            }
            if (!onBoard) {
                circleY += gravity;
                if (circleY >= h - ballRadius) {
                    circleY = h - ballRadius;
                }
            }
        }

        TCHAR scoreText[50];
        _stprintf_s(scoreText, sizeof(scoreText) / sizeof(TCHAR), _T("Score: %d"), totalBoardsPassed);
        settextcolor(WHITE);
        settextstyle(30, 0, _T("Arial"));
        outtextxy(10, 10, scoreText);

        if (totalBoardsPassed == 100 && !congratulationsShown) {
            settextcolor(GREEN);
            settextstyle(30, 0, _T("Arial"));
            outtextxy(200, 250, _T("Congratulations! You've scored 100 points!"));
            outtextxy(200, 300, _T("After 5 sec Press ESC to continue..."));
            congratulationsShown = true;
            gamePaused = true;
            int pauseDuration = 0;

            while (gamePaused) {
                FlushBatchDraw();
                Sleep(30);
                pauseDuration++;
                if (pauseDuration >= 150) {
                    gamePaused = false;
                }
                if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                    gamePaused = false;
                }
            }

            congratulationsShown = false;
        }

        for (int j = 0; j < lives; j++) {
            drawHeart(50 + j * 30, 50, 20);
        }

        if (circleY < 50 + ballRadius || circleY >= h - ballRadius) {
            lives--;
            if (lives <= 0) {
                break;
            }
            circleY = board[0].y - ballRadius;
            circleX = board[0].x + board[0].width / 2;
            continue;
        }

        if (GetAsyncKeyState(VK_LEFT) & 0x8000 && circleX - ballRadius > 0) {
            circleX -= 10;
        }
        if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && circleX + ballRadius < w) {
            circleX += 10;
        }
        if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
            bool onBoard = false;
            for (i = 0; i < 6; i++) {
                if (circleX >= board[i].x && circleX <= board[i].x + board[i].width &&
                    circleY + ballRadius >= board[i].y && circleY + ballRadius <= board[i].y + bh) {
                    onBoard = true;
                    break;
                }
            }
            if (onBoard && !isJumping && lives > 0) {
                isJumping = true;
                jumpStartY = circleY;
            }
        }

        FlushBatchDraw();
        Sleep(30);
    }

    if (lives <= 0) {
        TCHAR loseMessage[100];
        _stprintf_s(loseMessage, sizeof(loseMessage) / sizeof(TCHAR), _T("Your game is over! Score: %d"), totalBoardsPassed);
        settextcolor(RGB(255, 0, 0));
        settextstyle(30, 0, _T("Arial"));
        outtextxy(200, 300, loseMessage);
        settextcolor(WHITE);
        _stprintf_s(loseMessage, sizeof(loseMessage) / sizeof(TCHAR), _T("Press Enter to Restart or Esc to Quit"));
        outtextxy(200, 350, loseMessage);
        FlushBatchDraw();

        while (true) {
            if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
                lives = 3;
                gameLoop();
                break;
            }
            if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
                break;
            }
        }
    }

    closegraph();
}

int main() {
    startGame();
    return 0;
}
