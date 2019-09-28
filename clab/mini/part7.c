#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "part7.h"

/* In part7.c you will implement a sorted binary tree
 A binary tree is a tree in which each node has 
 at most two children (referred to as left and right).
 Each node also holds a user value (which is a C string in 
 this lab).  In a sorted binary tree, for each node n, n's left child's
 value is less than n and n's right child's value is greater than n.
 Binary trees can be complicated if one is to keep them balanced.
 They are much simpler if we don't care about balancing. You will 
 implement the simple, unbalanced binary in this lab.
 Specifically, after inserting "a", "d", "b", "e" (in the order specified),
 your binary tree will look like this simplified representation:
     "a"
        \
	"d"
	/ \
     "b"  "e"
*/

// alloc_node allocates a new tree node 
// and initializes its fields properly.
// Note: before implementing this function, you must decide what 
// fields the tree node should contain and complete the 
// type definition of tnode_t in part7.h
static tnode_t *
alloc_node() 
{
	tnode_t *new_node = malloc(sizeof(tnode_t));
	new_node->data = NULL; 
	new_node->left = NULL;
	new_node->right = NULL;
	return new_node;

}

// init_tree initializes the tree by allocating its root node and 
// returning it
tnode_t *
init_tree()
{
	return alloc_node();
}

// insert stores a string (s) in the sorted binary tree,
// specified by its root (tree).
// It returns 0 if string (s) has been successfully inserted 
// (i.e. no duplicates)
// It returns 1 if the string has not been inserted (due to duplicates)
// Hint: you can use strcmp(..) to compare two NULL-terminated C strings
// type "man strcmp" to learn how to use it.
int
insert(tnode_t *tree, char *s)
{ 
	//string not inserted due to duplicates  
	if (tree->data == s) {
		return 1;
	}

	else if (tree->data == NULL) {
		tree->data = s; 
		return 0; 
	}
	else {
		//tnode_t *temp = alloc_node(); 
		//temp->data = s; 
		if (tree->data > s) {
			if (tree->left != NULL) {
				insert(tree->left, s); 
				//return insert(tree->left, s); 
			}
			else {          
				tnode_t *temp = alloc_node(); 
				temp->data = s;  
				tree->left = temp; 
			}
		}
		else if (tree->data < s) {
			if (tree->right != NULL) {
				insert(tree->right, s);
				//return insert(tree->right,s);
			}
			else { 
				tnode_t *temp = alloc_node(); 
				temp->data = s;
				tree->right = temp; 
			}
		}
		return 0; 
	}
	return 1; 
}


// pre-order performs a preorder traversal of the sorted binary tree
// and store the sequence of strings visited along the way in 
// the array argument (result) whose allocated size is result_max
// It returns the number of strings stored in the result array.
// (Our tester part7_harness.c will allocate an array whose size (result_max) is 
// larger than the number of strings to be stored.)
// In preorder traversal, you print the value in node n first, and then 
// visit n's left child, and then visit n's right child.
// For example, for the tree below, the preorder traversal result is "a" "d" "b" "e"
//     "a"
//       \
//	"d"
//	/ \
//    "b"  "e"
//
// Note: you are free to write an auxilary function and use it here.
//int count = 0; 

int
preorder(tnode_t *tree, char **result, int result_max)
{
	int num = 1;
 	if(tree==NULL || tree->data==NULL)
 		return 0;
 	
 	*result=(char *) malloc(sizeof(char));
 	*result=tree->data;
 	result++;
 
 	num+=preorder(tree->left, result, result_max);
 	result+=num-1;
 
 	num+= preorder(tree->right, result, result_max);
 	return num;
 }


// inorder performs an inorder traversal of the sorted binary tree
// and store the sequence of strings visited along the way in 
// the array argument (result) whose allocated size is result_max
// It returns the number of strings stored in the result array.
// (Our tester part7_harness.c will allocate an array whose size (result_max) is 
// larger than the number of strings to be stored.)
// In inorder traversal, you visit node n's left child first, then print the value in node n, and then 
// visit node n's right child.
// For example, for the tree below, the inorder traversal result is "a" "b" "d" "e"
//     "a"
//       \
//	"d"
//	/ \
//    "b"  "e"
//
// Note: you are free to write an auxilary function and use it here.
int
inorder(tnode_t *tree, char **result, int max_result)
{

	if(tree==NULL || tree->data==NULL)
 		return 0;
 	
 	int num = inorder(tree->left, result, max_result);
 	result += num;
 
 	*result = (char *) malloc(sizeof(char));
 	*result = tree->data;
 	result++;
 	num++;
 
 	num += inorder(tree->right, result, max_result);
 	return num;
}

// del_tree de-allocates all nodes in the tree
// you should do not de-allocate the strings stored inside each node
void
del_tree(tnode_t *tree)
{
	if (tree != NULL) {
		if (tree->left != NULL) {
			del_tree(tree->left); 
		}
		if (tree->right != NULL) {
			del_tree(tree->right); 
		}
	free(tree);
	}
}
