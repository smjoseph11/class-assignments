#include <ncurses.h>
#include <unistd.h>
#include "Queue.h"

//----------------------------------------------------

#define START_ROW (20)
#define START_COL (20)
#define SLEEP_MICROSECONDS ((1.0 / 20.0) * 1000000.0)

//----------------------------------------------------

// Simple storage for a row, column pair.
struct V2 {
  int row, col;
};

//----------------------------------------------------

// Some of this ncurses code is, unfortunately, copied from
// the man page without full understanding.
void configureNcurses()
{
  initscr();
  cbreak();
  noecho();
  nonl();
  intrflush(stdscr, FALSE);
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  curs_set(0);
  start_color();
  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);
  init_pair(3, COLOR_YELLOW, COLOR_BLACK);
  init_pair(4, COLOR_BLUE, COLOR_BLACK);
  init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(6, COLOR_CYAN, COLOR_BLACK);
  init_pair(7, COLOR_WHITE, COLOR_BLACK);
}

//----------------------------------------------------

// Draw centipede with sequence of foreground colors, or with
// background color, depending on "erase" flag.
// (Pass centipede by value to exercise copy constructor.)
void drawCentipede(Queue<V2> centipede, bool erase)
{
  int drawCharacter;
  int colorNumber;
  V2 currentPosition, dummyPosition;
  Queue<V2> centipedeCopy;

  // Make a copy of centipede to be consumed during drawing.
  // (Exercises assignment operator.)
  centipedeCopy = centipede;
  
  // Prepare to draw or erase, as requested.
  if (erase)
    drawCharacter = ' ';
  else
    drawCharacter = ' ' | A_REVERSE;

  // Consume centipede copy to obtain data for drawing.
  for (int i = 0; i < QUEUE_SIZE; ++i) {
    colorNumber = 1 + (i % 7);
    currentPosition = centipede.pushAndPop(dummyPosition);
    attron(COLOR_PAIR(colorNumber));
    mvaddch(currentPosition.row, currentPosition.col,
            drawCharacter);
    attroff(COLOR_PAIR(colorNumber));
  }
}

//----------------------------------------------------

// Update position based on arrow key input.
void updatePosition(V2& position, int inputChar)
{
    switch (inputChar) {
    case KEY_UP:
      --position.row;
      break;
    case KEY_DOWN:
      ++position.row;
      break;
    case KEY_LEFT:
      --position.col;
      break;
    case KEY_RIGHT:
      ++position.col;
      break;
    default:
      // (Ignore all other keys.)
      break;
    }
}

//----------------------------------------------------

main()
{
  Queue<V2> centipede;
  int currentDirection;
  int inputChar;
  V2 currentHead;

  // Configure ncurses.
  configureNcurses();

  // Fill queue (all centipede segments at start position).
  currentHead.row = START_ROW;
  currentHead.col = START_COL;
  for (int i = 0; i < QUEUE_SIZE; ++i)
    centipede.pushAndPop(currentHead);

  // Draw instructions and initial centipede.
  attron(COLOR_PAIR(2));
  mvaddstr(1, 3, "USE ARROW KEYS TO MOVE, CTRL-C TO QUIT");
  attroff(COLOR_PAIR(2));
  drawCentipede(centipede, false);
  refresh();

  // Process input until killed.
  currentDirection = KEY_RIGHT;
  while (true) {
    // Show current state, then check for input.
    usleep(SLEEP_MICROSECONDS);
    inputChar = getch();
    if (inputChar != ERR)
      currentDirection = inputChar;

    // When input received, erase old centipede.
    drawCentipede(centipede, true);

    // Then use new input to update centipede.
    updatePosition(currentHead, currentDirection);
    centipede.pushAndPop(currentHead);

    // Then draw new centipede, and refresh.
    drawCentipede(centipede, false);
    refresh();
  }

  // Clean up ncurses.  (Re-display cursor.)
  curs_set(1);
}

//----------------------------------------------------
