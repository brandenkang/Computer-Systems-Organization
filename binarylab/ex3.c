//Implement a C function called ex3 such that it achieves 
//the same functionality as the machine code of objs/ex3_sol.o
//Note: you need to figure out ex3()'s function signature yourself; 
//the signature is not void ex3()
  
#include <assert.h>
int
ex3(char *a, char *b, int n) {
	int i = 0;
	while (i < n) {
		char tmp = a[i]; 
		a[i] = b[i]; 
		b[i] = tmp; 
		i++;
	}
	return i; 
}

