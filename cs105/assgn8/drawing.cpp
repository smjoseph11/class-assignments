//----------------------------------------------------

#include "drawing.h"
#include <ncurses.h>

#define SPRITE_ROWS (14)
#define SPRITE_COLS (14)

//----------------------------------------------------

// Takes colors with values in [0,255] and calls init_color()
// using values converted into [0, 1000] as ncurses expects.
// (I'll take a chance and assume I don't have to range check
// after conversion under these limited circumstances.)
static void initNcursesColor
(short color, int r, int g, int b)
{
  int ncurses_r = r * (1000.0 / 255.0);
  int ncurses_g = g * (1000.0 / 255.0);
  int ncurses_b = b * (1000.0 / 255.0);
  init_color(color, ncurses_r, ncurses_g, ncurses_b);
}

//----------------------------------------------------

static void drawSprite
(int startRow, int startCol,
 char data[SPRITE_ROWS][SPRITE_COLS + 1])
{
  int colorNumber;
  chtype drawChar;

  for (int row = 0; row < SPRITE_ROWS; ++row) {
    for (int col = 0; col < SPRITE_COLS; ++col) {
      // Set color and character to draw.
      switch (data[row][col]) {
      case 'K': // black
        colorNumber = 1;
        drawChar = ' ';
        break;
      case 'W': // white
        colorNumber = 7;
        drawChar = ' ' | A_REVERSE;
        break;
      case 'B': // blue
        colorNumber = 4;
        drawChar = ' ' | A_REVERSE;
        break;
      case 'R': // red
        colorNumber = 1;
        drawChar = ' ' | A_REVERSE;
        break;
      case 'M': // magenta
        colorNumber = 5;
        drawChar = ' ' | A_REVERSE;
        break;
      case 'C': // cyan
        colorNumber = 6;
        drawChar = ' ' | A_REVERSE;
        break;
      case 'Y': // yellow
        colorNumber = 3;
        drawChar = ' ' | A_REVERSE;
        break;
      }
      // Move to position and draw character.
      attron(COLOR_PAIR(colorNumber));
      mvaddch(startRow + row, startCol + (col * 2), drawChar);
      mvaddch(startRow + row, startCol + (col * 2) + 1, drawChar);
      attroff(COLOR_PAIR(colorNumber));
    }
  }
}

//----------------------------------------------------

// Configure ncurses, and set up the initial window.
void beginDrawing()
{
  initscr();
  cbreak();
  noecho();
  nonl();
  intrflush(stdscr, FALSE);
  keypad(stdscr, TRUE);
  nodelay(stdscr, FALSE);
  curs_set(0);
  start_color();
  if (can_change_color()) {
    initNcursesColor(COLOR_WHITE, 231, 235, 223);
    initNcursesColor(COLOR_BLUE, 35, 63, 139);
    initNcursesColor(COLOR_RED, 253, 59, 17);
    initNcursesColor(COLOR_MAGENTA, 253, 195, 212);
    initNcursesColor(COLOR_CYAN, 73, 223, 202);
    initNcursesColor(COLOR_YELLOW, 255, 191, 87);
  }
  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);
  init_pair(3, COLOR_YELLOW, COLOR_BLACK);
  init_pair(4, COLOR_BLUE, COLOR_BLACK);
  init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(6, COLOR_CYAN, COLOR_BLACK);
  init_pair(7, COLOR_WHITE, COLOR_BLACK);
  attron(COLOR_PAIR(2));
  mvaddstr(1, 2, "(PRESS ANY KEY TO QUIT)");
  attroff(COLOR_PAIR(2));
  refresh();
}

//----------------------------------------------------

void drawShadow(int row, int col)
{
  static char data[SPRITE_ROWS][SPRITE_COLS + 1] = {
    "KKKKKRRRRKKKKK",
    "KKKRRRRRRRRKKK",
    "KKRRRRRRRRRRKK",
    "KRRRWWRRRRWWRK",
    "KRRWWWWRRWWWWK",
    "KRRWWBBRRWWBBK",
    "RRRWWBBRRWWBBR",
    "RRRRWWRRRRWWRR",
    "RRRRRRRRRRRRRR",
    "RRRRRRRRRRRRRR",
    "RRRRRRRRRRRRRR",
    "RRRRRRRRRRRRRR",
    "RRKRRRKKRRRKRR",
    "RKKKRRKKRRKKKR"
  };

  drawSprite(row, col, data);
}

//----------------------------------------------------

void drawSpeedy(int row, int col)
{
  static char data[SPRITE_ROWS][SPRITE_COLS + 1] = {
    "KKKKKMMMMKKKKK",
    "KKKMMMMMMMMKKK",
    "KKMMMMMMMMMMKK",
    "KMMMMMMMMMMMMK",
    "KMMWWMMMMWWMMK",
    "KMWWWWMMWWWWMK",
    "MMWWWWMMWWWWMM",
    "MMWBBWMMWBBWMM",
    "MMMBBMMMMBBMMM",
    "MMMMMMMMMMMMMM",
    "MMMMMMMMMMMMMM",
    "MMMMMMMMMMMMMM",
    "MMKMMMKKMMMKMM",
    "MKKKMMKKMMKKKM"
  };

  drawSprite(row, col, data);
}

//----------------------------------------------------

void drawBashful(int row, int col)
{
  static char data[SPRITE_ROWS][SPRITE_COLS + 1] = {
    "KKKKKCCCCKKKKK",
    "KKKCCCCCCCCKKK",
    "KKCCCCCCCCCCKK",
    "KCWWCCCCWWCCCK",
    "KWWWWCCWWWWCCK",
    "KBBWWCCBBWWCCK",
    "CBBWWCCBBWWCCC",
    "CCWWCCCCWWCCCC",
    "CCCCCCCCCCCCCC",
    "CCCCCCCCCCCCCC",
    "CCCCCCCCCCCCCC",
    "CCCCCCCCCCCCCC",
    "CCKCCCKKCCCKCC",
    "CKKKCCKKCCKKKC"
  };

  drawSprite(row, col, data);
}

//----------------------------------------------------

void drawPokey(int row, int col)
{
  static char data[SPRITE_ROWS][SPRITE_COLS + 1] = {
    "KKKKKYYYYKKKKK",
    "KKKBBYYYYBBKKK",
    "KKWBBWYYWBBWKK",
    "KYWWWWYYWWWWYK",
    "KYYWWYYYYWWYYK",
    "KYYYYYYYYYYYYK",
    "YYYYYYYYYYYYYY",
    "YYYYYYYYYYYYYY",
    "YYYYYYYYYYYYYY",
    "YYYYYYYYYYYYYY",
    "YYYYYYYYYYYYYY",
    "YYYYYYYYYYYYYY",
    "YYKYYYKKYYYKYY",
    "YKKKYYKKYYKKKY"
  };

  drawSprite(row, col, data);
}

//----------------------------------------------------

void drawErrorMessage(int row, int col)
{
  attron(COLOR_PAIR(2));
  mvaddstr(row + 6, col + 5, "POLYMORPHISM ERROR");
  attroff(COLOR_PAIR(2));
}

//----------------------------------------------------

void endDrawing()
{
  // Wait for character to quit.
  getch();
  // Clean up.
  endwin();
}

//----------------------------------------------------
