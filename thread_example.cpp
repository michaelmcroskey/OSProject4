#include <iostream>
#include <unistd.h>
#include <pthread.h>
using namespace std;

// our global condition variable and mutex
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

// our predicate values.
bool finished = false;
int jobs_waiting = 0;
int jobs_completed = 0;

// our thread proc
static void *worker_proc(void* p)
{
	intptr_t id = (intptr_t)p;  // our id
	size_t n_completed = 0;     // our job completion count

	// always latch prior to eval'ing predicate vars.
	pthread_mutex_lock(&mtx);
	while (!finished)
	{
		// wait for finish or work-waiting predicate
		while (!finished && jobs_waiting == 0)
			pthread_cond_wait(&cv, &mtx);

		// we own the mutex, so we're free to look at, modify
		//  etc. the values(s) that we're using for our predicate
		if (finished)
			break;

		// must be a job_waiting, reduce that number by one, then
		//  unlock the mutex and start our work. Note that we're
		//  changing the predicate (jobs_waiting is part of it) and
		//  we therefore need to let anyone that is monitoring know.
		--jobs_waiting;
		pthread_cond_broadcast(&cv);
		pthread_mutex_unlock(&mtx);

		// DO WORK HERE (this just runs a lame summation)
		for (int i=0,x=0;i<1048576; x += ++i);
		++n_completed;

		// finished work latch mutex and setup changes
		pthread_mutex_lock(&mtx);
		++jobs_completed;
		pthread_cond_broadcast(&cv);
	}

	// final report
	cout << id << ": jobs completed = " << n_completed << endl;

	// we always exit owning the mutex, so unlock it now. but
	//  let anyone else know they should be quitting as well.
	pthread_cond_broadcast(&cv);
	pthread_mutex_unlock(&mtx);
	return p;
}

// sets up a batch of work and waits for it to finish.
void run_batch(int num)
{
	pthread_mutex_lock(&mtx);
	jobs_waiting = num;
	jobs_completed = 0;
	pthread_cond_broadcast(&cv);

	// wait or all jobs to complete.
	while (jobs_completed != num)
		pthread_cond_wait(&cv, &mtx);

	// we own this coming out, so let it go.
	pthread_mutex_unlock(&mtx);
}

// main entry point.
int main()
{
	// number of threads in our crew
	static const size_t N = 7;
	pthread_t thrds[N] = {0};

	// startup thread crew.
	intptr_t id = 0;
	for (size_t i=0; i<N; ++i)
		pthread_create(thrds + i, NULL, worker_proc, (void*)(++id));

	// run through batches. each batch is one larger
	//  than the prior batch. this should result in some
	//  interesting job-counts per-thread.
	for (int i=0; i<64; ++i)
		run_batch(i);

	// flag for shutdown state.
	pthread_mutex_lock(&mtx);
	finished = true;
	pthread_cond_broadcast(&cv);
	pthread_mutex_unlock(&mtx);
	for (size_t i=0; i<N; pthread_join(thrds[i++], NULL));

	return 0;
}