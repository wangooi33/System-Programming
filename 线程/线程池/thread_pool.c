#include "thread_pool.h"


void handler(void *arg)
{
	printf("[%u] is ended.\n",
		(unsigned)pthread_self());

	pthread_mutex_unlock((pthread_mutex_t *)arg);
}


//线程要执行的任务
void *routine(void *arg)
{
	//调试
	#ifdef DEBUG
	printf("[%u] is started.\n",
		(unsigned)pthread_self());
	#endif

	//把需要传递给线程任务的参数进行备份
	thread_pool *pool = (thread_pool *)arg;
	struct task *p;

	while(1)
	{
		/*
		** push a cleanup functon handler(), make sure that
		** the calling thread will release the mutex properly
		** even if it is cancelled during holding the mutex.
		**
		** NOTE:
		** pthread_cleanup_push() is a macro which includes a
		** loop in it, so if the specified field of codes that 
		** paired within pthread_cleanup_push() and pthread_
		** cleanup_pop() use 'break' may NOT break out of the
		** truely loop but break out of these two macros.
		** see line 61 below.
		*/
		//================================================//
		pthread_cleanup_push(handler, (void *)&pool->lock);   ///???
		pthread_mutex_lock(&pool->lock);
		//================================================//

		// 1, no task, and is NOT shutting down, then wait
		while(pool->waiting_tasks == 0 && !pool->shutdown)
		{
			pthread_cond_wait(&pool->cond, &pool->lock);
		}

		// 2, no task, and is shutting down, then exit
		if(pool->waiting_tasks == 0 && pool->shutdown == true)
		{
			pthread_mutex_unlock(&pool->lock);
			pthread_exit(NULL); // CANNOT use 'break';
		}

		// 3, have some task, then consume it
		p = pool->task_list->next;
		pool->task_list->next = p->next;
		pool->waiting_tasks--;

		//================================================//
		pthread_mutex_unlock(&pool->lock);
		pthread_cleanup_pop(0);
		//================================================//

		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
		(p->do_task)(p->arg);
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

		free(p);
	}

	pthread_exit(NULL);
}


//初始化线程池
bool init_pool(thread_pool *pool, unsigned int threads_number)
{
	//初始化互斥锁
	pthread_mutex_init(&pool->lock, NULL);

	//初始化条件量
	pthread_cond_init(&pool->cond, NULL);

	//销毁标志 
	pool->shutdown = false; //不销毁

	//给链表的节点申请堆内存
	pool->task_list = malloc(sizeof(struct task)); 

	//申请堆内存，用于存储创建出来的线程的ID
	pool->tids = malloc(sizeof(pthread_t) * MAX_ACTIVE_THREADS);

	//错误处理，对malloc进行错误处理
	if(pool->task_list == NULL || pool->tids == NULL)
	{
		perror("allocate memory error");
		return false;
	}

	//对任务链表中的节点的指针域进行初始化
	pool->task_list->next = NULL;

	//设置线程池中线程数量的最大值
	pool->max_waiting_tasks = MAX_WAITING_TASKS;
	
	//设置等待线程处理的任务的数量为0，说明现在没有任务
	pool->waiting_tasks = 0;

	//设置线程池中活跃的线程的数量
	pool->active_threads = threads_number;

	int i;

	//循环创建活跃线程
	for(i=0; i<pool->active_threads; i++)
	{
		//创建线程  把线程的ID存储在申请的堆内存
		if(pthread_create(&((pool->tids)[i]), NULL,
					routine, (void *)pool) != 0)
		{
			perror("create threads error");
			return false;
		}

		//用于调试
		#ifdef DEBUG
		printf("[%u]:[%s] ==> tids[%d]: [%u] is created.\n",
			(unsigned)pthread_self(), __FUNCTION__,
			i, (unsigned)pool->tids[i]);
		#endif
	}

	return true;
}

//先线程池的任务链表中添加任务
bool add_task(thread_pool *pool,
	      void *(*do_task)(void *arg), void *arg)
{
	//给任务链表节点申请内存
	struct task *new_task = malloc(sizeof(struct task));
	if(new_task == NULL)
	{
		perror("allocate memory error");
		return false;
	}

	new_task->do_task = do_task;
	new_task->arg = arg;			
	new_task->next = NULL;			//指针域设置为NULL

	//============ LOCK =============//
	pthread_mutex_lock(&pool->lock);
	//===============================//

	//说明要处理的任务的数量大于能处理的任务数量
	if(pool->waiting_tasks >= MAX_WAITING_TASKS)
	{
		pthread_mutex_unlock(&pool->lock);

		fprintf(stderr, "too many tasks.\n");
		free(new_task);

		return false;
	}
	
	struct task *tmp = pool->task_list;

	//遍历链表，找到单向链表的尾节点
	while(tmp->next != NULL)
		tmp = tmp->next;

	//把新的要处理的任务插入到链表的尾部  尾插
	tmp->next = new_task;

	//要处理的任务的数量+1
	pool->waiting_tasks++;

	//=========== UNLOCK ============//
	pthread_mutex_unlock(&pool->lock);
	//===============================//

	//调试
	#ifdef DEBUG
	printf("[%u][%s] ==> a new task has been added.\n",
		(unsigned)pthread_self(), __FUNCTION__);
	#endif

	//唤醒第一个处于阻塞队列中的线程
	pthread_cond_signal(&pool->cond);
	return true;
}

//向线程池加入新线程
int add_thread(thread_pool *pool, unsigned additional_threads)
{
	//判断需要添加的新线程的数量是否为0
	if(additional_threads == 0)
		return 0;

	//计算线程池中总线程的数量
	unsigned total_threads =
			pool->active_threads + additional_threads;

	int i, actual_increment = 0;

	//
	for(i = pool->active_threads;i < total_threads && i < MAX_ACTIVE_THREADS;i++)
	{
		//创建新线程
		if(pthread_create(&((pool->tids)[i]),
					NULL, routine, (void *)pool) != 0)
		{
			perror("add threads error");

			// no threads has been created, return fail
			if(actual_increment == 0)
				return -1;

			break;
		}
		actual_increment++; 

		#ifdef DEBUG
		printf("[%u]:[%s] ==> tids[%d]: [%u] is created.\n",
			(unsigned)pthread_self(), __FUNCTION__,
			i, (unsigned)pool->tids[i]);
		#endif
	}

	//记录此时线程池中活跃线程的总数
	pool->active_threads += actual_increment;
	return actual_increment;
}

int remove_thread(thread_pool *pool, unsigned int removing_threads)
{
	if(removing_threads == 0)
		return pool->active_threads;

	int remaining_threads = pool->active_threads - removing_threads;
	remaining_threads = remaining_threads > 0 ? remaining_threads : 1;

	int i;
	for(i=pool->active_threads-1; i>remaining_threads-1; i--)
	{
		errno = pthread_cancel(pool->tids[i]);

		if(errno != 0)
			break;

		#ifdef DEBUG
		printf("[%u]:[%s] ==> cancelling tids[%d]: [%u]...\n",
			(unsigned)pthread_self(), __FUNCTION__,
			i, (unsigned)pool->tids[i]);
		#endif
	}

	if(i == pool->active_threads-1)
		return -1;
	else
	{
		pool->active_threads = i+1;
		return i+1;
	}
}

bool destroy_pool(thread_pool *pool)
{
	// 1, activate all threads
	pool->shutdown = true;
	pthread_cond_broadcast(&pool->cond);

	// 2, wait for their exiting
	int i;
	for(i=0; i<pool->active_threads; i++)
	{
		errno = pthread_join(pool->tids[i], NULL);
		if(errno != 0)
		{
			printf("join tids[%d] error: %s\n",
					i, strerror(errno));
		}
		else
			printf("[%u] is joined\n", (unsigned)pool->tids[i]);
		
	}

	// 3, free memories
	free(pool->task_list);
	free(pool->tids);
	free(pool);

	return true;
}
