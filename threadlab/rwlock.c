#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <assert.h>
#include "rwlock.h"

/* rwl implements a reader-writer lock.
 * A reader-write lock can be acquired in two different modes, 
 * the "read" (also referred to as "shared") mode,
 * and the "write" (also referred to as "exclusive") mode.
 * Many threads can grab the lock in the "read" mode.  
 * By contrast, if one thread has acquired the lock in "write" mode, no other 
 * threads can acquire the lock in either "read" or "write" mode.
 */

//helper function
static inline int
cond_timedwait(pthread_cond_t *c, pthread_mutex_t *m, const struct timespec *expire)
{
	int r; 
	if (expire != NULL)  {
		r = pthread_cond_timedwait(c, m, expire);
	} else
		r = pthread_cond_wait(c, m);
	assert(r == 0 || r == ETIMEDOUT);
       	return r;
}

//rwl_init initializes the reader-writer lock 
void
rwl_init(rwl *l)
{
	l->readers = 0; 
	l->writer = 0; 
	l->rWaiters = 0; 
	l->wWaiters = 0; 
	pthread_mutex_init(&(l->mutex), NULL); 
	pthread_cond_init(&(l->rwcond), NULL); 

}

//rwl_nwaiters returns the number of threads *waiting* to acquire the lock
//Note: it should not include any thread who has already grabbed the lock
int
rwl_nwaiters(rwl *l) 
{
	//Your code here
	return (l->rWaiters + l->wWaiters);

	
}

//rwl_rlock attempts to grab the lock in "read" mode
//if lock is not grabbed before absolute time "expire", it returns ETIMEDOUT
//else it returns 0 (when successfully grabbing the lock)
int
rwl_rlock(rwl *l, const struct timespec *expire)
{
	//Your code here
	int time;
	pthread_mutex_lock(&(l->mutex));
	while (l->writer > 0 || l->wWaiters > 0){ 
		l->rWaiters ++;
		time = cond_timedwait(&(l->rwcond), &(l->mutex), expire);
		if (time == ETIMEDOUT) {
			l->rWaiters --; //reader stops waiting since timedout
			pthread_mutex_unlock(&(l->mutex));
			return time;
		}
		l->rWaiters --; 
	}
	l->readers++; 
	pthread_mutex_unlock(&(l->mutex));
	return 0;
}

//rwl_runlock unlocks the lock held in the "read" mode
void
rwl_runlock(rwl *l)
{
	//Your code here
	pthread_mutex_lock(&(l->mutex));
	l->readers--; 
	if (l->readers == 0 && l->writer ==0) {
		pthread_cond_broadcast(&(l->rwcond));
	}
	pthread_mutex_unlock(&(l->mutex));
}

//rwl_wlock attempts to grab the lock in "write" mode
//if lock is not grabbed before absolute time "expire", it returns ETIMEDOUT
//else it returns 0 (when successfully grabbing the lock)
int
rwl_wlock(rwl *l, const struct timespec *expire)
{
	//Your code here
	pthread_mutex_lock(&(l->mutex));
	int wait;
	while (l->writer == 1 || (l->readers) > 0) { 
		l->wWaiters++;
		wait = cond_timedwait(&(l->rwcond), &(l->mutex), expire);
		if (wait == ETIMEDOUT) {
			l->wWaiters --;
			pthread_mutex_unlock(&(l->mutex));
			return ETIMEDOUT;
		}
	}
	l->writer = 1;
	l->wWaiters --;
	pthread_mutex_unlock(&(l->mutex));
	return 0;
}

//rwl_wunlock unlocks the lock held in the "write" mode
void
rwl_wunlock(rwl *l)
{
	//Your code here
	pthread_mutex_lock(&(l->mutex));
	l->writer--;
	pthread_cond_broadcast(&(l->rwcond));
	pthread_mutex_unlock(&(l->mutex));
}
