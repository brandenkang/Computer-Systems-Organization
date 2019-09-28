#include <stdlib.h>
#include <assert.h>
#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

#include "htable.h"

#define MAX_COLLISION 10
rwl elock;
pthread_mutex_t *m;
/* htable implements a hash table that handles collisions by chaining.
 * It contains an array, where each slot stores the head of a singly-linked list.
 * if the length of some chain is found to be longer than MAX_COLLISION after an insertion, the 
 * htable is resized by doubling the array size. 
 */
// calculate modulo addition
int
madd(int a, int b)
{
	return ((a+b)>BIG_PRIME?(a+b-BIG_PRIME):(a+b));
}

// calculate modulo multiplication
int
mmul(int a, int b)
{
	return ((a*b) % BIG_PRIME);
}


//hashcode returns the hashcode for a C string
int
hashcode(char *s) {
	int p = 0;
	int i = 0;
	while (s[i] != '\0') {
		p  = madd(mmul(p, 256), s[i]);
		i++;
	}
	return p;
}

//is_prime returns true if n is a prime number
int
is_prime(int n) {
	if (n <= 3) {
		return 1;
	}
	if ((n % 2) == 0) {
		return 0;
	}
	for (int i = 3; i < n; i+=2) {
		if ((n % i) == 0) {
			return 0;
		}
	}
	return 1;
}

static int
get_prime(int min) {
	int n = min;
	while (1) {
		if (is_prime(n)) {
			return n;
		}
		n++;	
	}
}

//htable_init returns a new hash table that's been initialized
void
htable_init(htable *ht, int sz, int allow_resize) {
	ht->size = get_prime(sz + 1);
	ht->store = (node **)malloc(sizeof(node *)*ht->size);
	ht->allow_resize = allow_resize;
	assert(ht->store);

	m = (pthread_mutex_t *)malloc((ht->size)*sizeof(pthread_mutex_t));
	for (int i = 0; i<(ht->size); i++) {
		pthread_mutex_init(&m[i], NULL);
	}
	rwl_init(&elock);
	bzero(ht->store, sizeof(node *)*ht->size);

}

//htable_size returns the number of slots in the hash table
int
htable_size(htable *ht) {
	int sz = ht->size;
	return sz;
}

void
free_linked_list(node *n) {
	if (n != NULL) {
		free_linked_list(n->next);
		free(n);
	}
}

//htable_destroy destroys the htable, freeing the memory associated with its fields
void
htable_destroy(htable *ht) {
	for (int i = 0; i < ht->size; i++) {
		free_linked_list(ht->store[i]);
		pthread_mutex_destroy(&m[i]);
	}
	free(ht->store);
}

//htable_resize increases the size of an existing htable in order to control
//max number of collsions.
static void
htable_resize(htable *ht) {
	rwl_wlock(&elock, NULL);
	int newSize = get_prime(2*ht->size);
	for (int i = 0; i < ht->size; i++) {
		pthread_mutex_destroy(&m[i]); 
	}
	m = realloc(m, (sizeof(pthread_mutex_t) * newSize));
	for (int i = 0; i < newSize; i++) {
		pthread_mutex_init(&m[i], NULL);
	}

	node **newStore = (node **)malloc(sizeof(node *)*newSize);
	assert(newStore != NULL);
	bzero(newStore, sizeof(node *)*newSize);

	for (int i = 0; i < ht->size; i++) {
		node *current = ht->store[i];
		while (current) {
			node *n = current;
			current = current->next;
			int slot = n->hashcode % newSize;
			if (newStore[slot] != NULL) {
				n->next = newStore[slot];
			} else {
				n->next = NULL;
			}
		       	newStore[slot] = n;
		}
	}
	ht->size = newSize;
	free(ht->store);
	ht->store = newStore;
	rwl_wunlock(&elock);
}

//htable_insert inserts the key, val tuple into the htable. If the key already 
//exists, it returns 1 indicating failure.  Otherwise, it inserts the new val and returns 0. 
int
htable_insert(htable *ht, char *key, void *val) {

	rwl_rlock(&elock, NULL);
	int hcode = hashcode(key);	
    int slot = hcode % ht->size;
	int collision = 0;

	pthread_mutex_lock(&m[slot]);
	node *curr = ht->store[slot];
	node *prev = NULL; 

	while (curr) {
		if ((curr->hashcode == hcode) && (strcmp(curr->key, key) == 0)) {
			pthread_mutex_unlock(&m[slot]);
			rwl_runlock(&elock);
			return 1; 
		}
		prev = curr;
		curr = curr->next;
		collision++;
	}

	//allocate a node to store key/value tuple
	node *n = (node *)malloc(sizeof(node));
	n->hashcode = hcode;
	n->key = key;
	n->val = val;
	n->next = NULL;

	if (prev == NULL) {
		ht->store[slot] = n;
	}else {
		prev->next = n;
	}
	if (ht->allow_resize && collision >= MAX_COLLISION) {
		pthread_mutex_unlock(&m[slot]); 
		rwl_runlock(&elock); 
		htable_resize(ht);
	}
	else {
		pthread_mutex_unlock(&m[slot]);
		rwl_runlock(&elock);
	}

	return 0; //success
}

//htable_lookup returns the corresponding val if key exists
//otherwise it returns NULL.
void *
htable_lookup(htable *ht, char *key) {
	rwl_rlock(&elock, NULL);
	int hCode = hashcode(key);
    int slot = hCode % ht->size;
	pthread_mutex_lock(&m[slot]);
	node *curr = ht->store[slot];
	while (curr) {
		if ((curr->hashcode == hCode) && (strcmp(curr->key, key) == 0)) {
			pthread_mutex_unlock(&m[slot]);
			rwl_runlock(&elock);
			return curr->val;
		}
		curr = curr->next;
	}
	pthread_mutex_unlock(&m[slot]);
	rwl_runlock(&elock);
	return NULL;
}
