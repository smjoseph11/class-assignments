int n;
int& foo() {
    n = 10/2;
    return n;
}
int main(){
  foo();
}
