//Implement a C function called ex1 such that it achieves 
//the same functionality as the machine code of objs/ex1_sol.o
//Note: you need to figure out ex1()'s function signature yourself; 
//the signature is not void ex1()
  
#include <assert.h>

long
ex1(long a, long b, long c) {
	return (a+b!=c)? 0:1; 
}