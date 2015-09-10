#ifndef STACK_H
#define STACK_H

class Node{
    int value;
    Node *next;
    public:
       Node(){
           value = 0; 
           next = 0;
       };
       ~Node(){
       };
       int getValue(){
           return value;
       }
       void setValue(int val){
           value = val;
       }
       Node *getNext(){
           return next;
       }
       void setNext(Node *n){
           next = n;
       }
};
class Stack{
    int height;
    Node *head;
    public:
        Stack();
        ~Stack();
        void push(int i);
        int pop();
        int peek();
        bool isEmpty();
        int getHeight();
};
#endif 
