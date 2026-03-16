#include <ncurses.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define DELAY 150000

typedef enum { LEFT, RIGHT, UP, DOWN } directionType;

typedef struct {
  int x;
  int y;
} Point;

int maxY = -1;
int maxX = -1;
int nextY = 0;
int nextX = 0;
int score = 0;
int tailLength = 5;
bool gameover = false;
directionType direction = RIGHT;
Point food;
Point snakeParts[255] = {};

void createFood(void) {
  food.x = (rand() % (maxX - 10));
  food.y = (rand() % (maxY - 5));
}

void shiftSnake(void) {
  Point tmp = snakeParts[tailLength - 1];

  for (int i = tailLength; i > 0; i--) {
    snakeParts[i] = snakeParts[i - 1];
  }

  snakeParts[0] = tmp;
}

void ncursesInit(void) {
  initscr();
  noecho();
  curs_set(FALSE);
  nodelay(stdscr, TRUE);
  keypad(stdscr, TRUE);
}

void init(void) {
  srand(time(NULL));
  direction = RIGHT;
  tailLength = 5;
  gameover = false;
  score = 0;

  clear();
  int j = 0;
  for (int i = tailLength; i >= 0; i--) {
    Point currentPoint;
    currentPoint.x = i;
    currentPoint.y = maxY / 2;

    snakeParts[j] = currentPoint;
    j++;
  }
  createFood();
  refresh();
}

void drawSnakePart(Point part) { mvprintw(part.y, part.x, "0"); }

void drawFood(Point food) { mvprintw(food.y, food.x, "o"); }

void drawScreen(void) {
  clear();
  wborder(stdscr, ACS_VLINE, ACS_VLINE, ACS_HLINE, ACS_HLINE, ACS_ULCORNER,
          ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER);

  if (gameover) {
    mvprintw(maxY / 2, maxX / 2, "GAME OVER");
  }

  for (int i = 0; i < tailLength - 1; i++) {
    drawSnakePart(snakeParts[i]);
  }

  drawFood(food);
  mvprintw(1, 2, "Score: %d", score);
  refresh();
  usleep(DELAY);
}

int main() {
  ncursesInit();
  getmaxyx(stdscr, maxY, maxX);
  init();
  int ch;

  while (1) {

    if (gameover) {
      sleep(2);
      init();
    }

    ch = getch();

    if ((ch == KEY_RIGHT) && (direction != RIGHT && direction != LEFT)) {
      direction = RIGHT;
    } else if ((ch == KEY_LEFT) && (direction != RIGHT && direction != LEFT)) {
      direction = LEFT;
    } else if ((ch == KEY_UP) && (direction != UP && direction != DOWN)) {
      direction = UP;
    } else if ((ch == KEY_DOWN) && (direction != UP && direction != DOWN)) {
      direction = DOWN;
    }

    nextY = snakeParts[0].y;
    nextX = snakeParts[0].x;

    if (direction == RIGHT) {
      nextX++;
    } else if (direction == LEFT) {
      nextX--;
    } else if (direction == UP) {
      nextY--;
    } else if (direction == DOWN) {
      nextY++;
    }

    if ((nextY >= maxY || nextY < 0) || (nextX >= maxX || nextX < 0)) {
      gameover = true;
    }

    if (food.y == nextY && food.x == nextX) {
      Point tail;
      tail.y = food.y;
      tail.x = food.x + tailLength;

      snakeParts[tailLength] = tail;

      if (tailLength < 255) {
        tailLength++;
      } else {
        tailLength = 5;
      }
      score += 5;
      createFood();
    } else {
      for (int i = 0; i < tailLength; i++) {
        if (nextX == snakeParts[i].x && nextY == snakeParts[i].y) {
          gameover = true;
          break;
        }
      }
      shiftSnake();
      snakeParts[0].x = nextX;
      snakeParts[0].y = nextY;
    }
    drawScreen();
  }

  endwin();

  return 0;
}
