/* NYU Computer Systems Organization Lab 2
 * Rabin-Karp Substring Matching
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <ctype.h>

#include "rkgrep.h"
#include "bloom.h"

#define PRIME 961748941

// calculate modulo addition, i.e. (a+b) % PRIME
long long
madd(long long a, long long b)
{
	return (a + b) % PRIME;
}

// calculate modulo substraction, i.e. (a-b) % PRIME
long long
msub(long long a, long long b)
{
	return (a>b)?(a-b):(a+PRIME-b);
}

// calculate modulo multiplication, i.e. (a*b) % PRIME
long long
mmul(long long a, long long b)
{
	return (a*b) % PRIME;
}

/* naive_substring_match returns number of positions in the document where
 * the pattern has been found.  In addition, it stores the first position 
 * where the pattern is found in the variable pointed to by first_match_ind
 *
 * Both its arguments "pattern" and "doc" are null-terminated C strings.
 */
int
naive_substring_match(const char *pattern, const char *doc, int *first_match_ind)
{
	int d_len = strlen(doc);
	int p_len = strlen(pattern);
	int a = d_len - p_len;
	int pos = 0;; 
	int unequal = 0;

	for(int i=0; i < a; i++){
		int j=0;
		int count = 0;
		if (j<p_len) {
			while(doc[i+j]==pattern[j]) {
				j++;
				count++;
			}
		}
		if(count==p_len){
			unequal++; 
			if(unequal==1){
				*first_match_ind=i;
			}
			pos++;
		}
	}
	return pos;
}

/* initialize the Rabin-karp hash computation by calculating 
 * and returning the RK hash over a charbuf of m characters, 
 * i.e. The return value should be 
 * 256^(m-1)*charbuf[0] + 256^(m-2)*charbuf[1] + ... + charbuf[m-1],
 * where 256^(m-1) means 256 raised to the power m-1.
 * Note that all operations *, +, - are modulo arithematic, so you 
 * should use the provided functions mmul, madd, msub.
 * (We use "long long" to represent an RK hash)
 */
long long
rkhash_init(const char *charbuf, int m, long long *h)
{
	long long val = 1;
	long long sum = 0;
	for (int i = m-1; i >= 0; i--) {
		sum = madd(mmul(val, charbuf[i]), sum);
		val = mmul(val, 256);
	}
	*h = val; 
	return sum; 
}


/* Given the rabin-karp hash value (curr_hash) over substring Y[i],Y[i+1],...,Y[i+m-1]
 * calculate the hash value over Y[i+1],Y[i+2],...,Y[i+m] = curr_hash * 256 - leftmost * h + rightmost
 * where h is 256 raised to the power m (and given as an argument).  
 * Note that *,-,+ refers to modular arithematic so you should use mmul, msub, madd.
 */
long long 
rkhash_next(long long curr_hash, long long h, char leftmost, char rightmost)
{
  curr_hash = msub(mmul(curr_hash,256), mmul(leftmost, h));
  curr_hash = madd(curr_hash, rightmost);
  return curr_hash;
}

/* rk_substring_match returns the number of positions in the document "doc" where
 * the "pattern" has been found, using the Rabin-karp substring matching algorithm.
 * Both pattern and doc are null-terminated C strings. The function also stores
 * the first position where pattern is found in the int variable pointed to by first_match_ind
 *
 * Note: You should implement the Rabin-Karp algorithm by completing the 
 * rkhash_init and rkhash_next functions and then use them here.
*/
int
rk_substring_match(const char *pattern, const char *doc, int *first_match_ind)
{
	long long p_len = strlen(pattern);
	long long d_len = strlen(doc); 
	long long h;
	long long h2; 

    long long cHash = rkhash_init(doc, p_len, &h);
    long long pHash = rkhash_init(pattern, p_len, &h2);

	int sum = 0;
	int unequal = 0;

    for (int i = 0; i < d_len - p_len; i++) { 
    	if (cHash == pHash) { 
    		unequal++; 
    		sum++;
    		if (unequal == 1) {
    			*first_match_ind = i;
    		}
    	}
    	cHash = rkhash_next(cHash, h, doc[i], doc[i + p_len]);
    }
	return sum;
}


/* rk_create_doc_bloom returns a pointer to a newly created bloom_filter. 
 * The new bloom filter is populated with all n-m+1 rabin-karp hashes for 
 * all the substrings of length m in "doc".
 * Hint: use the provided bloom_init() and your implementation of bloom_add() here.
 */
bloom_filter *
rk_create_doc_bloom(int m, const char *doc, int bloom_size)
{
	bloom_filter *bfilter = bloom_init(bloom_size);
	long long d_len = strlen(doc); 

	long long h;
	long long cHash = rkhash_init(doc, m, &h);

	for (int i = 0; (i+m-1)<d_len; i++) { 
		bloom_add(bfilter, cHash);
		cHash = rkhash_next(cHash, h, doc[i], doc[i + m]);
	}
	return bfilter;
}

/* rk_substring_match_using_bloom returns the total number of positions where "pattern" 
 * is found in "doc".  It performs the matching by first checking against the 
 * pre-populated bloom filter "bf" (which has been created by rk_create_doc_bloom on "doc")
 * If the pattern is not found in "bf", then the function immediately returns 0.
 * Otherwise, the function invokes rk_substring_match() function to find "pattern" in "doc".
*/
int
rk_substring_match_using_bloom(const char *pattern, const char *doc, bloom_filter *bf, int *first_match_ind)
{
	int d_len = strlen(doc); 
	int p_len = strlen(pattern); 
	if (d_len < p_len) {
		return 0; 
	}

	long long *ptrH = malloc(sizeof(long long *));
	long long pHash = rkhash_init(pattern,p_len,ptrH);

	if(bloom_query(bf,pHash)!=true) {
		 return 0;
	}
	else {
		int match = rk_substring_match(pattern, doc, first_match_ind);
  		return match;
	}
}
