#include <stdio.h>
#include <stdlib.h>

struct node
{
    char data;
    struct node *next;
};
struct node *nalloc(void)
{
    return (struct node *) malloc(sizeof(struct node));
};

struct node *head;
void
push(char c){
    struct node *newNode = nalloc();
    (*newNode).data = c;
    newNode->next = head;
    head = newNode;
}
char
pop(){
    char c = head->data;
    struct node *newHead = head->next;
    free(head);
    head = newHead;

    return c;
}
int
empty(){
    if(head == NULL)
        return 1;
    return 0;
}
int
main(int argc, char **argv){
    head = NULL;
    char line[20];
    char c;
    FILE *fp = fopen(argv[1], "r"); 
    if(fp == NULL){
        printf("File not found\n");
        return 0;  
    }
    while(fscanf(fp, "%s", line) != EOF){
        if(strcmp(line, "push") == 0){
            fscanf(fp, "%s", line);
            push(*line);
        }
        else if(strcmp(line, "pop") == 0 && !empty()){
            pop();
        }
        else{
          fprintf(stderr, "CANNOT POP, STACK IS EMPTY\n");
          exit(1);
        }
     
    }
    fclose(fp);
    struct node *temp = head;
    fp = fopen(argv[2], "w+");
    while(temp != NULL){
        fprintf(fp, "%c", temp->data);
        temp = temp->next;
        pop();
    }
    fclose(fp);
}
