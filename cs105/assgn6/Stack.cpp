#include <iostream>
#include <cstdlib>
using namespace std;
#include "Stack.h"

Stack::Stack(){
    height = 0;
    head = 0;
}
Stack::~Stack(){
      while(!isEmpty()){
          pop();
      }
}
void Stack::push(int i){
  height++;
  Node* n = new Node();
  n->setValue(i);
  n->setNext(head);
  head = n;
}
int Stack::pop(){
  height--;
  int i = head->getValue();
  Node* n = head->getNext();
  delete head;
  head = n;
  return i;
}
int Stack::peek(){
  int i = head->getValue();
  return i;
}
bool Stack::isEmpty(){
  if(height == 0)
      return true;
  return false;
}
int Stack::getHeight(){
  return height;
}
