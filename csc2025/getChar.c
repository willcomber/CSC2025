#include <stdio.h>

int main(void)
{
  char a [10];
  for(int i = 0; i < 10; i++){
     printf("Enter character number %d: ",(i+1));
     scanf(" %c", &a[i]); // need space before %c to fix bug
   }

   printf("You entered:");
  for(int i = 0; i < 10; i++){
     
	 printf("%c", a[i]);
  }
  printf("\n");
  return 0;
}