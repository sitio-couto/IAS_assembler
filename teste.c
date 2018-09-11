#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void){
	
	char *n = calloc(1, sizeof(char));
	char *x = NULL;
	
	free(x);
	
	printf("%i", n);
		
	return 0;
}
//16254576
