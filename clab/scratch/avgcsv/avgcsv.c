#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *file; 
char lineA[2000]; 
double averageA[2000]; 

int main(int argc, char *argv[]) {
	file = fopen(argv[1], "r"); 
	if (file==NULL) {
		return 0;
	} 
	else {
		int lineNum = 0; 
		int counter; 
		char *tok;// = (char *)malloc(sizeof(char));
		while (fgets(lineA, 2000, file)!=NULL) {
			counter = 0;
			tok = strtok(lineA, ";"); 
			while (tok!=NULL) {
				averageA[counter] += atof(tok); 
				tok = strtok(NULL, ";");
				counter++; 
			}
			lineNum++; 
		}
		for (int i = 0; i < counter; i++) {
			averageA[i] = averageA[i] / lineNum; 
			printf("%.2lf;", averageA[i]); 
		}
		printf("\n");
	} 
	return 0; 
} 