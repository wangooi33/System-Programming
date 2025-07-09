/*
 * 线程过多会带来调度开销，频繁创建和销毁线程也会带来极大的开销，不易调度和控制。
 *
 * 线程池：通过预先创建好一定数量的线程，将任务提交给线程池，由线程池中的空闲线程来执行任务，从而避免频繁创建/销毁线程的开销。
 * 
 * 
*/


#include "thread_pool.h"

void *mytask(void *arg)
{
	int n = (int)arg;

	printf("[%u][%s] ==> job will be done in %d sec...\n",
		(unsigned)pthread_self(), __FUNCTION__, n);

	sleep(n);

	printf("[%u][%s] ==> job done!\n",
		(unsigned)pthread_self(), __FUNCTION__);

	return NULL;
}

void *count_time(void *arg)
{
	int i = 0;
	while(1)
	{
		sleep(1);
		printf("sec: %d\n", ++i);
	}
}


int main(void)
{
	pthread_t a;
	pthread_create(&a, NULL, count_time, NULL);

	// 1, initialize the pool
	thread_pool *pool = malloc(sizeof(thread_pool));
	init_pool(pool, 2);

	// 2, throw tasks
	printf("throwing 3 tasks...\n");
	add_task(pool, mytask, (void *)(rand()%10));
	add_task(pool, mytask, (void *)(rand()%10));
	add_task(pool, mytask, (void *)(rand()%10));

	// 3, check active threads number
	printf("current thread number: %d\n",
			remove_thread(pool, 0));
	sleep(9);

	// 4, throw tasks
	printf("throwing another 2 tasks...\n");
	add_task(pool, mytask, (void *)(rand()%10));
	add_task(pool, mytask, (void *)(rand()%10));

	// 5, add threads
	add_thread(pool, 2);

	sleep(5);

	// 6, remove threads
	printf("remove 3 threads from the pool, "
	       "current thread number: %d\n",
			remove_thread(pool, 3));

	// 7, destroy the pool
	destroy_pool(pool);
	return 0;
}
