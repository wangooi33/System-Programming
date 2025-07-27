/*
 * 本程序演示如何使用自定义的线程池库：
 * - 初始化线程池；
 * - 向线程池中添加任务；
 * - 动态增加/减少线程数量；
 * - 最终销毁线程池；
 */

/************************************************************************/

/*
 * 线程过多会带来调度开销，进而影响缓存局部性和整体性能。
 * 频繁创建和销毁线程也会带来极大的开销。
 * 通常我们更加关心的是任务可以并发执行，并不想管理线程的创建，销毁和调度。
 * 通过将任务处理成队列，交由线程池（Thread Pool）统一执行，可以提升任务的执行效率。
 * 
 * 线程池指的是将线程放入一个类似缓冲区的池子中，对于生命周期很短的任务而言，
 * 什么时候需要线程就从池子中捞出来干活，干完活就放回池子且让线程保持整装待命，
 * 并通过精巧的设计使得池子中的线程数量可以动态地发生变化，
 * 让线程既可以应对并发性需求，又不会浪费系统资源，这种设计思路就是线程池。
 * 
 * 采用线程池维护多个线程，等待监督管理者分配可并发执行的任务。
 * 这种做法避免了处理任务时创建销毁线程开销的代价，也避免了线程数量膨胀导致的过分调度问题，
 * 保证了对内核的充分利用。
 *
 * *********************************************************************
 * 
 * 线程被创建出来之后，都处于睡眠态，它们实际上是进入了条件量的等待队列中。
 * 而任务都被放入一个链表，被互斥锁保护起来。下面是线程池里面线程们的一生：
 *
 * 1.被创建
 * 2.写遗书（准备好退出处理函数，防止在持有一把锁的状态中死去）
 * 3.试图持有互斥锁（等待任务）
 * 4.判断是否有任务，若无则进入条件量等待队列睡眠，若有则进入第5步
 * 5.从任务链表中取得一个任务
 * 6.释放互斥锁
 * 7.销毁遗书（将备用的退出处理函数弹出，避免占用内存）
 * 8.执行任务，完毕之后重回第2步
 * 
 * 任务实质上是用户需要交给线程池执行的函数，为了方便线程们执行，
 * 一般做法是将函数（即函数指针）及其参数存入一个任务节点，并将节点链成一个链表。
 * 
*/


#include "thread_pool.h"

// 模拟任务函数：线程执行后休眠 n 秒
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

// 时间计数线程，每秒打印一次
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
	// 创建时间计数线程，用于测试时观察线程执行时长
	pthread_create(&a, NULL, count_time, NULL);

	// 1. 初始化线程池（创建2个工作线程）
	thread_pool *pool = malloc(sizeof(thread_pool));
	init_pool(pool, 2);

	// 2. 向线程池添加任务（3个任务）
	printf("throwing 3 tasks...\n");
	add_task(pool, mytask, (void *)(rand() % 10));
	add_task(pool, mytask, (void *)(rand() % 10));
	add_task(pool, mytask, (void *)(rand() % 10));

	// 3. 查看当前线程数量（此处传入 0，表示仅获取数量）
	printf("current thread number: %d\n",
			remove_thread(pool, 0));

	sleep(9);  // 等待任务执行完成

	// 4. 再次向线程池添加任务（2个任务）
	printf("throwing another 2 tasks...\n");
	add_task(pool, mytask, (void *)(rand() % 10));
	add_task(pool, mytask, (void *)(rand() % 10));

	// 5. 动态添加线程（增加2个线程）
	add_thread(pool, 2);

	sleep(5);  // 等待任务执行一段时间

	// 6. 动态移除线程（移除3个线程），并显示剩余线程数
	printf("remove 3 threads from the pool, "
	       "current thread number: %d\n",
			remove_thread(pool, 3));

	// 7. 销毁线程池，释放资源
	destroy_pool(pool);
	return 0;
}
