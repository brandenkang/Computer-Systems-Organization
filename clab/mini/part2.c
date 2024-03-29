#include <assert.h>

// Swap the values of the integer variables pointed to by p1 and
// p2.  After the function returns, the variable pointed to by p1
// should have the value once pointed to by p2, and vice-versa.
//
// You may use a temporary variable or do an XOR swap.
void
swap(int *p1, int *p2)
{
	assert(p1 != 0);
	assert(p2 != 0);
	// TODO: Your code here.
	int tmp = *p1; 
	*p1 = *p2; 
	*p2 = tmp;
	//assert(0);
}

// bubble_sort implements the bubble sort algorithm.
// https://en.wikipedia.org/wiki/Bubble_sort
// It is given an integer array "arr" of "n" elements. 
// Upon return, the integer array should contain the sorted numbers
// in increasing order. 
// You should use the previous swap function when implementing bubble_sort
void
bubble_sort(int *arr, int n)
{
        // TODO: Your code here.
		for (int i = 0; i<n-1; i++) {
			for (int k = 0; k < n-i-1; k++) {
				if (arr[k] > arr[k+1]) {
					swap(&arr[k], &arr[k+1]); 
				}
			}
		}
       // assert(0);
}