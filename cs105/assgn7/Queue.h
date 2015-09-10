#ifndef STACK_H
#define STACK_H
#define QUEUE_SIZE 30

template<class Type>
class Queue {
    public:
        Queue();
        Queue(Queue &source);
        ~Queue();
        void operator=(Queue &source);
        Type pushAndPop(Type &elem);
    private:
        Type *elems;
};

template<class Type>
Queue<Type>:: Queue(){
    elems = new Type[QUEUE_SIZE];
}
template<class Type>
Queue<Type>:: Queue(Queue &source){
    if(source.elems !=0){
        elems = new Type[QUEUE_SIZE];
        for(int i=0; i<QUEUE_SIZE; i++){
            elems[i] = source.elems[i];
        }
    }
}
template<class Type>
Queue<Type>:: ~Queue(){
  delete[] elems;
}
template<class Type>
void Queue<Type>::operator=(Queue &source){
    if (this == &source) return;
    delete[] elems;
    if(source.elems !=0) { 
        elems = new Type[QUEUE_SIZE];
        for(int i=0; i<QUEUE_SIZE; i++){
        elems[i] = source.elems[i];
        }
    }
}
template<class Type>
Type Queue<Type>:: pushAndPop(Type &elem){
    Type retelem = elems[QUEUE_SIZE-1];
    for(int i=QUEUE_SIZE-1; i>0; i--){
        elems[i] = elems[i-1];
    }
    elems[0] = elem;
    return retelem;
}

#endif
