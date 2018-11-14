#include <stdio.h>
#include <stdlib.h>

int main(void){
	printf("Please choose a number between 1 and 100 inclusive that the computer will now guess\n");
	printf("The computer will now guess a number, if it is too low enter l if too high enter h or if correct enter c\n\n");
	
	int found = 0;
	int guess;
	int max = 100;
	int min = 1;
	int counter =0;
	char hl; 
	
	
	while(counter<7 && found == 0){
		guess = (max + min) / 2;
		printf("Is your number %d? ",guess);
		scanf(" %c",&hl);
		if(hl == 'c'){
			printf("The computer guessed correctly after %d attempts\n", (counter +1));
			found = 1;
		}else if(hl == 'l'){
			printf("Guess was too low, %d guesses left\n",(7-counter));
			counter++;
			min = guess + 1;
		}else if(hl == 'h'){
			printf("Guess was too high, %d guesses left\n",(7-counter));
			counter++;
			max = guess - 1;
		}else{
			printf("invalid response\n");
		}
		
	}
	if(counter == 7){
		printf("The computer was unable to guess your number");
	}
}