#include "thread_pool.h"

/*
 * 清理函数 handler()
 * 在线程被取消（pthread_cancel）时自动调用，
 * 主要作用是释放线程持有的互斥锁，防止死锁。
 * 
 * @param arg 传入的是互斥锁的地址，强制转换后解锁
 */
void handler(void *arg)
{
	printf("[%u] is ended.\n",
		(unsigned)pthread_self());

	pthread_mutex_unlock((pthread_mutex_t *)arg);
}


/*
 * 线程主执行函数 routine()
 * 每个线程从线程池启动时执行此函数，持续循环等待任务，
 * 直到线程池关闭（shutdown == true）且无剩余任务时退出。
 * 
 * 核心步骤：
 * 1. 上锁保护共享任务队列和线程池状态；
 * 2. 使用条件变量等待任务或销毁信号；
 * 3. 取出并移除链表头部任务，更新等待任务数；
 * 4. 解锁，执行任务（执行期间禁止线程取消以防止资源泄漏）；
 * 5. 释放任务结点内存；
 * 6. 继续循环等待下一个任务。
 * 
 * 注意：
 * - 使用 pthread_cleanup_push/pop 保证即使线程被取消，
 *   互斥锁也能被释放，避免死锁；
 * - pthread_cond_wait 会自动释放互斥锁并阻塞线程，返回时重新加锁；
 * - 任务链表头是一个哨兵节点，不存任务，真正任务从 next 开始。
 */
void *routine(void *arg)
{
#ifdef DEBUG
	printf("[%u] is started.\n", (unsigned)pthread_self());
#endif

	thread_pool *pool = (thread_pool *)arg;
	struct task *p;

	while (1)
	{
		/*
		 * 设置线程取消清理函数，确保持有锁时被取消能正确释放锁
		 * pthread_cleanup_push/pop 是宏，成对使用。
		 * 这里传入 handler 函数和锁地址，确保退出时释放锁。
		 */
		pthread_cleanup_push(handler, (void *)&pool->lock);

		// 申请互斥锁，保护后续对共享资源操作的原子性
		pthread_mutex_lock(&pool->lock);

		/*
		 * 进入条件等待循环：
		 * - 当前没有任务可执行；
		 * - 线程池未设置销毁标志（继续等待新任务）
		 * 
		 * 使用 pthread_cond_wait 挂起线程，等待 add_task 的唤醒。
		 * pthread_cond_wait 内部自动释放锁，等待唤醒后重新加锁。
		 */
		while (pool->waiting_tasks == 0 && !pool->shutdown)
		{
			pthread_cond_wait(&pool->cond, &pool->lock);
		}

		/*
		 * 条件判断：
		 * - 没有等待任务且线程池标记销毁，
		 * - 说明线程池关闭，且无剩余任务可执行，线程应退出。
		 */
		if (pool->waiting_tasks == 0 && pool->shutdown == true)
		{
			pthread_mutex_unlock(&pool->lock); // 解锁
			pthread_exit(NULL);                 // 线程退出，不能用 break
		}

		/*
		 * 有任务可执行，取出第一个任务节点（链表头后一个）
		 * 断开链表连接，等待任务数量减一。
		 * 这里的 task_list 是哨兵节点，真正任务链表从 next 开始。
		 */
		p = pool->task_list->next;
		pool->task_list->next = p->next;
		pool->waiting_tasks--;

		// 解锁，允许其他线程继续访问任务链表
		pthread_mutex_unlock(&pool->lock);

		/*
		 * 取消清理代码块弹出，此时不执行清理函数（传0）
		 * 线程进入任务执行状态
		 */
		pthread_cleanup_pop(0);

		/*
		 * 禁止线程被取消，保证任务执行期间不被中断，避免资源泄漏
		 * 任务函数调用完成后，恢复线程可取消状态。
		 */
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
		(p->do_task)(p->arg); // 执行任务函数
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

		// 任务节点释放，防止内存泄漏
		free(p);
	}

	pthread_exit(NULL); // 正常退出（实际不会到这步）
}


/*
 * 初始化线程池 init_pool()
 * 创建初始线程，初始化同步原语及链表头结点，
 * 设置线程池状态标志和计数变量。
 * 
 * @param pool 线程池指针（外部需malloc）
 * @param threads_number 初始线程数，不能超过最大限制
 * @return true 初始化成功，false 失败
 */
bool init_pool(thread_pool *pool, unsigned int threads_number)
{
	// 初始化互斥锁，默认属性
	pthread_mutex_init(&pool->lock, NULL);

	// 初始化条件变量，默认属性
	pthread_cond_init(&pool->cond, NULL);

	pool->shutdown = false; // 线程池未关闭

	// 创建哨兵任务节点，不保存任务
	pool->task_list = malloc(sizeof(struct task));
	if (pool->task_list == NULL)
	{
		perror("allocate memory error");
		return false;
	}
	pool->task_list->next = NULL;

	// 为线程ID数组分配内存，容量最大线程数
	pool->tids = malloc(sizeof(pthread_t) * MAX_ACTIVE_THREADS);
	if (pool->tids == NULL)
	{
		free(pool->task_list);
		perror("allocate memory error");
		return false;
	}

	// 设定线程池参数初始值
	pool->max_waiting_tasks = MAX_WAITING_TASKS;
	pool->waiting_tasks = 0;
	pool->active_threads = threads_number;

	// 创建指定数量的工作线程
	for (int i = 0; i < pool->active_threads; i++)
	{
		if (pthread_create(&pool->tids[i], NULL, routine, (void *)pool) != 0)
		{
			perror("create threads error");
			return false;
		}

#ifdef DEBUG
		printf("[%u]:[%s] ==> tids[%d]: [%u] is created.\n",
			(unsigned)pthread_self(), __FUNCTION__,
			i, (unsigned)pool->tids[i]);
#endif
	}

	return true;
}


/*
 * 添加任务到线程池 add_task()
 * - 先创建任务节点，填充函数指针和参数
 * - 加锁保护任务链表
 * - 判断任务队列是否已满
 * - 尾插法将任务节点加入链表末尾
 * - 等待任务数+1
 * - 解锁
 * - 唤醒等待线程
 * 
 * @return true 添加成功，false 失败（如内存分配失败或任务过多）
 */
bool add_task(thread_pool *pool,
			  void *(*do_task)(void *arg), void *arg)
{
	// 分配新任务结点内存
	struct task *new_task = malloc(sizeof(struct task));
	if (new_task == NULL)
	{
		perror("allocate memory error");
		return false;
	}

	new_task->do_task = do_task;
	new_task->arg = arg;
	new_task->next = NULL;

	pthread_mutex_lock(&pool->lock);

	// 判断等待任务是否达到上限，避免无限制增长
	if (pool->waiting_tasks >= MAX_WAITING_TASKS)
	{
		pthread_mutex_unlock(&pool->lock);
		fprintf(stderr, "too many tasks.\n");
		free(new_task);
		return false;
	}

	// 找到任务链表尾部，插入新任务
	struct task *tmp = pool->task_list;
	while (tmp->next != NULL)
		tmp = tmp->next;
	tmp->next = new_task;

	pool->waiting_tasks++;

	pthread_mutex_unlock(&pool->lock);

#ifdef DEBUG
	printf("[%u][%s] ==> a new task has been added.\n",
		   (unsigned)pthread_self(), __FUNCTION__);
#endif

	// 唤醒至少一个等待线程执行任务
	pthread_cond_signal(&pool->cond);
	return true;
}


/*
 * 向线程池增加线程 add_thread()
 * - 计算目标线程数，限制不超过最大活跃线程数
 * - 循环创建线程，记录实际创建成功数量
 * - 创建失败则停止创建，返回已创建线程数或 -1
 * - 更新线程池活跃线程计数
 * 
 * @return 实际成功创建线程数，失败返回 -1
 */
int add_thread(thread_pool *pool, unsigned additional_threads)
{
	if (additional_threads == 0)
		return 0;

	unsigned total_threads = pool->active_threads + additional_threads;
	int actual_increment = 0;

	for (int i = pool->active_threads; i < total_threads && i < MAX_ACTIVE_THREADS; i++)
	{
		if (pthread_create(&pool->tids[i], NULL, routine, (void *)pool) != 0)
		{
			perror("add threads error");

			if (actual_increment == 0)
				return -1; // 无线程创建成功，返回错误

			break; // 终止后续线程创建
		}
		actual_increment++;

#ifdef DEBUG
		printf("[%u]:[%s] ==> tids[%d]: [%u] is created.\n",
			   (unsigned)pthread_self(), __FUNCTION__,
			   i, (unsigned)pool->tids[i]);
#endif
	}

	pool->active_threads += actual_increment;
	return actual_increment;
}


/*
 * 从线程池移除线程 remove_thread()
 * - 若传入0，则仅返回当前活跃线程数量，不做移除
 * - 计算移除后剩余线程数，保证至少保留1个线程
 * - 从线程ID数组末尾开始取消线程（pthread_cancel）
 * - 取消失败即停止，返回 -1
 * - 更新活跃线程计数
 * 
 * @return 移除后的线程数量，失败返回 -1
 */
int remove_thread(thread_pool *pool, unsigned int removing_threads)
{
	if (removing_threads == 0)
		return pool->active_threads;

	int remaining_threads = pool->active_threads - removing_threads;
	if (remaining_threads < 1)
		remaining_threads = 1; // 至少保留一个线程

	int i;
	for (i = pool->active_threads - 1; i >= remaining_threads; i--)
	{
		errno = pthread_cancel(pool->tids[i]);
		if (errno != 0)
			break;

#ifdef DEBUG
		printf("[%u]:[%s] ==> cancelling tids[%d]: [%u]...\n",
			   (unsigned)pthread_self(), __FUNCTION__,
			   i, (unsigned)pool->tids[i]);
#endif
	}

	if (i == pool->active_threads - 1)
		return -1; // 无线程取消成功
	else
	{
		pool->active_threads = i + 1;
		return i + 1;
	}
}


/*
 * 销毁线程池 destroy_pool()
 * - 设定关闭标志
 * - 广播唤醒所有等待线程
 * - 等待所有线程退出（pthread_join）
 * - 释放任务链表和线程ID数组内存
 * - 释放线程池结构体内存
 * 
 * @return true 表示销毁完成
 */
bool destroy_pool(thread_pool *pool)
{
	// 标记线程池关闭，所有线程看到后退出循环
	pool->shutdown = true;

	// 唤醒所有阻塞在条件变量上的线程，防止死锁
	pthread_cond_broadcast(&pool->cond);

	// 等待所有活跃线程结束，回收资源
	for (int i = 0; i < pool->active_threads; i++)
	{
		errno = pthread_join(pool->tids[i], NULL);
		if (errno != 0)
		{
			printf("join tids[%d] error: %s\n",
				   i, strerror(errno));
		}
		else
		{
			printf("[%u] is joined\n", (unsigned)pool->tids[i]);
		}
	}

	// 释放任务链表哨兵节点内存
	free(pool->task_list);

	// 释放线程ID数组内存
	free(pool->tids);

	// 释放线程池结构体内存（调用者传入时注意）
	free(pool);

	return true;
}
