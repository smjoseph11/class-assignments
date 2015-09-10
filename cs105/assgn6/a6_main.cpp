//-----------------------------------------------

#include <iostream>
using namespace std;

#include "Stack.h"

//-----------------------------------------------

#define NUM_RODS (3)
#define NUM_DISCS (5)
#define MAX_DISC_WIDTH ((2 * NUM_DISCS) - 1)
#define DISC_DISPLAY_WIDTH (MAX_DISC_WIDTH + 2)
#define BACKGROUND_CHAR (' ')
#define ROD_CHAR ('|')
#define DISC_CHAR ('=')
#define BASE_CHAR ('_')

//-----------------------------------------------

// Prints disc of given size into fixed-width space.
// (Prints rod if size is 0.)
void printDisc(int size) {
  char discChars[DISC_DISPLAY_WIDTH];
  int centerIndex = DISC_DISPLAY_WIDTH / 2;

  for (int i = 0; i < DISC_DISPLAY_WIDTH; ++i)
    discChars[i] = BACKGROUND_CHAR;

  if (size == 0) {
    discChars[centerIndex] = ROD_CHAR;
  } else {
    for (int i = 0; i < size; ++i) {
      discChars[centerIndex - i] = DISC_CHAR;
      discChars[centerIndex + i] = DISC_CHAR;
    }
  }

  for (int i = 0; i < DISC_DISPLAY_WIDTH; ++i)
    cout << discChars[i];
}

//-----------------------------------------------

// Display full contents of all towers.
// (Moves each tower onto and off of a temporary stack to
// access disc sizes for printing.)
void displayTowers(Stack towers[]) {
  Stack tempTowers[NUM_RODS];
  int altitude;
  
  // Always print empty tops of stacks.
  cout << endl;
  cout << BACKGROUND_CHAR;
  for (int i = 0; i < NUM_RODS; ++i)
    printDisc(0);
  cout << BACKGROUND_CHAR;
  cout << endl;

  // Display parts of towers that may contain discs.
  for (int i = 0; i < NUM_DISCS; ++i) {
    // Print one level (of all towers).
    cout << BACKGROUND_CHAR;
    for (int j = 0; j < NUM_RODS; ++j) {
      // If this tower is tall enough to be displayed at this
      // point, read from it (by copying onto temporary
      // tower), otherwise, print empty rod.
      altitude = NUM_DISCS - i;
      if (altitude <= towers[j].getHeight()) {
        tempTowers[j].push(towers[j].pop());
        printDisc(tempTowers[j].peek());
      }
      else {
        printDisc(0);
      }
    }
    cout << BACKGROUND_CHAR;
    cout << endl;
  }

  // Return temporary tower contents to real towers.
  for (int i = 0; i < NUM_RODS; ++i) {
    while (!tempTowers[i].isEmpty()) {
      towers[i].push(tempTowers[i].pop());
    }
  }

  // Display base.
  cout << BACKGROUND_CHAR;
  for (int i = 0; i < DISC_DISPLAY_WIDTH * NUM_RODS; ++i)
    cout << BASE_CHAR;
  cout << BACKGROUND_CHAR;
  cout << endl;
  cout << endl;
}

//-----------------------------------------------

int main(int argc, char** argv)
{
  Stack towers[NUM_RODS];
  int from, to;

  // Produce initial configuration.
  for (int i = 0; i < NUM_DISCS; ++i) {
    towers[0].push(NUM_DISCS - i);
  }

  // Process moves until quit.
  while(1) {
    // Display current state.
    displayTowers(towers);
    cout << endl;
    
    // Begin getting input.
    cout << "ENTER TWO ROD NUMBERS (FROM, TO) SEPARATED "
         << "BY A SPACE (OR 0 TO QUIT): ";
    cin >> from;

    // Check for quit.
    if (from == 0) {
      cout << endl;
      break;
    }

    // If still here, finish getting input.
    cin >> to;
    cout << endl;

    // Switch from [1,n] indices to [0, n-1].
    --from;
    --to;

    // Check for bad moves.
    if ((from < 0 || from >= NUM_RODS) ||
        (to < 0 || to >= NUM_RODS)) {
      cout << "ERROR: ROD NUMBER OUT OF RANGE." << endl;
      continue;
    }
    if (towers[from].isEmpty()) {
      cout << "ILLEGAL MOVE FROM EMPTY ROD." << endl;
      continue;
    }
    if (!towers[to].isEmpty()) {
      if (towers[from].peek() > towers[to].peek()) {
        cout << "ILLEGAL MOVE: LARGER OVER SMALLER." << endl;
        continue;
      }
    }

    // If still here, make the move.
    towers[to].push(towers[from].pop());
  }
}

//-----------------------------------------------
