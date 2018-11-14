#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	int user_num;
	printf("Enter number for file writing: ");
    scanf(" %d", &user_num);
	
	if(user_num < 1){
		printf("%d is too low\n",user_num);
	}else{
		
	
		FILE *out_file = fopen("out.txt", "w"); // write only 
           
		// test for files not existing. 
		if (out_file == NULL) {   
			printf("Error! Could not open file\n"); 
			exit(-1); 
		}
		
		for(int i =1; i <= user_num; i++){
			fprintf(out_file, "%d\n", i); // write to file 
		}
		fclose(out_file); // close file
	}
	return 0;
}