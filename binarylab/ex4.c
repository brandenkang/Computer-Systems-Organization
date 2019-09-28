//Implement a C function called ex4 such that it achieves 
//the same functionality as the machine code of objs/ex4_sol.o
//Note: you need to figure out ex4()'s function signature yourself; 
//the signature is not void ex4()
  
#include <assert.h>

int
ex4(int x) {
	int sum = 0; 
	if (x<=1) {
		return x; 
	}
	else {
		sum += (ex4(x-1)); 
		sum += (ex4(x-2)); 
	}
	return sum;
}