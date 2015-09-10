#include <iostream>
#include <cstdlib>
using namespace std;

class Game {
    int randomNumber;
    public:
        void generateNumber(int lower, int upper);
        bool tryGuess(int guess){
          if(guess == randomNumber)  
            return true;
          else{
            if(guess > randomNumber){
                cout << "guess was too high\n";
                return false;
            }
            else{
              if(guess < randomNumber){
                cout << "guess was too low\n";
                return false;
              }
            }
          }
        }
};

void Game::generateNumber(int lower, int upper){
    srand((unsigned)time(0));
    randomNumber = rand() % (upper - lower + 1) + lower;
}

main() {
    int lower;
    int upper;
    int guess;
    cout << "specify low and high limits (respectively) ";
    cin >> lower >> upper;
    Game game;
    game.generateNumber(lower, upper);
    cout << "Guess the number: ";
    cin >>guess;
    while(game.tryGuess(guess) == false){
        cout << "Guess the number: ";
        cin >> guess;

    }
    cout << "You win!\n";
}
