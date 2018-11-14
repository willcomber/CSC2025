#include <stdio.h>
#include <stdlib.h>

int rfactorial(int n);
int factorial(int n);

int main(int argc, char *argv[]) {
   printf("%d\n", atoi(argv[1]));
   printf("%d\n", rfactorial(atoi(argv[1])));
   printf("%d\n", factorial(atoi(argv[1])));
   return 0;
}

int rfactorial(int n) {
   if(n <= 1) {
       return n;
   }
   else {
       return n * rfactorial(n - 1);
   }
}

int factorial(int n){
   int factorial = 1;

   for(int i=n; i>1; i--){
       factorial *= i;
   }
   return factorial;
}