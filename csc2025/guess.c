#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
   srand(time(NULL)); // randomize seed
   int rando = (rand () % 100) + 1;
   // printf ("%d\n", rando);

   int counter = 0;
   int guess;
   int found = 0; // no boolean so using 0 for false and 1 for true

   while(counter<7 && found == 0){
       printf("Guess a number, %d attempts left\n", (7-counter));
       scanf(" %d", &guess);
       if (guess == rando){
           found = 1; //1 is for true
           printf("Well done you have found the number which was %d\n", rando);
       }
       else {
           counter++;
           if (guess > rando) {
               printf("You guessed too high\n");
           }
           else {
               printf("You guessed too low\n");
           }
       }
   }
   if (counter == 7){
       printf("You need to think harder\n");
   }
   return 0;
}