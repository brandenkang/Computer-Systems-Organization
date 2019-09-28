#include <stdio.h>
#include <stdlib.h>

FILE *file;
int n;

int fOpen(char *fileName) {
	file=fopen(fileName,"r");
	FILE *tmp=fopen(fileName, "r");
	char ch=fgetc(tmp);
	n=0;
	while(ch == '.' || ch =='x'){
		n++;
		ch=fgetc(tmp);
	}
	fclose(tmp);

	return 1;
}

int compute(int i, int j, int a[n][n]) {
	int neighbX = a[i][(n+j-1)%n] + a[(i+n-1)%n][j] + a[(i+1)%n][(j+1)%n] + a[i][(j+1)%n]\
		+ a[(n+i-1)%n][(n+j-1)%n] + a[(i+n-1)%n][(j+1)%n] + a[(i+1)%n][j]\
		+ a[(i+1)%n][(n+j-1)%n];

	if (a[i][j]!=0) {
		if (neighbX != 2 && neighbX != 3) {
			return 0; 
		}
		else {
			return 1; 
		}
	}
	else {
		if (neighbX!=3) {
			return 0; 
		}
		else {
			return 1;
		}
	}
}

void printChar(int a[n][n]) { 
	int c = 0;
	int r = 0;
	char ch;
	ch = fgetc(file);
	while (ch != EOF) {
		if (ch!='x' && ch!= '.') {
			c = 0;
			r++; 
			ch=fgetc(file); 
		}

		else if (ch == 'x') {
			a[r][c] = 1; 
			ch = fgetc(file); 
			c++;
		}

		else {
			a[r][c]=0;
			ch=fgetc(file); 
			c++; 
		}
	}
}

void printGen(int a[n][n]) {
	for (int i=0; i<n; i++) {
		for (int j=0; j<n; j++) {
			if (a[i][j]==0) {
				printf("%c", '.'); 
			}
			else {
				printf("%c", 'x'); 
			}
		}
		printf("\n");
	}
	printf("\n");
}

void copy(int a[n][n]){
	int temp[n][n];
	for(int i=0; i<n; i++) {
		for(int j=0; j<n; j++) {
			temp[i][j]=compute(i, j, a);
		}
	}
	for(int i=0; i<n; i++) {
		for(int j=0; j<n; j++){
			a[i][j]=temp[i][j];
		}
	}
}

int main(int argc, char **argv){
	fOpen(argv[1]); 
	int a[n][n]; 
	printChar(a); 
	printf("Initial Generation\n"); 
	printGen(a); 
	int gen = atoi(argv[2]); 
	int z = 0; 
	while (z<gen) {
		copy(a);
		printf("Generation %d>\n", z+1); 
		printGen(a); 
		z++; 	
	}
	fclose(file); 
	return 0; 
}