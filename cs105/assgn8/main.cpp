#include "drawing.h"

class Sprite { 
    public:
       void setPosition(int r, int c){
           row = r;
           col = c;
       }
       int getRow(){
           return row;
       }
       int getCol(){
           return col;
       }
       virtual void draw(){
           drawErrorMessage(row, col);
       }

    private:
        int row;
        int col;
};
class Shadow : public Sprite {
    public:
        void draw();
};
class Speedy : public Sprite {
    public:
        void draw();
};
class Bashful : public Sprite {
    public:
        void draw();
};
class Pokey : public Sprite {
    public:
        void draw();
};
void Shadow::draw() {drawShadow(getRow(),getCol());}
void Speedy::draw() {drawSpeedy(getRow(),getCol());}
void Bashful::draw() {drawBashful(getRow(),getCol());}
void Pokey::draw() {drawPokey(getRow(),getCol());}

main(){
    Sprite *array[4];
    beginDrawing();
    array[0] = new Shadow;
    array[0]->setPosition(5,10);
    array[1] = new Speedy;
    array[1]->setPosition(5,44);
    array[2] = new Bashful;
    array[2]->setPosition(22,44);
    array[3] = new Pokey;
    array[3]->setPosition(22,10);
    array[0]->draw();
    array[1]->draw();
    array[2]->draw();
    array[3]->draw();
    for(int i =0; i<4; i++)
        delete array[i];
    endDrawing();
}
