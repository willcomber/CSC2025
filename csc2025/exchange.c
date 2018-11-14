#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
   double xr = atof(argv[1]); // exchange rate
   double gbp =1; // pound start value

   printf("UK Pounds  EUROS\n");
   printf(" %8.2f%7.2f\n",gbp,gbp*xr); // output for 1gbp

   for(gbp = 10; gbp < 101; gbp += 10){ // loop increasing 10 each iterationn
       printf(" %8.2f%7.2f\n",gbp,gbp*xr); // output current rate exchange
   }

   return 0;
}