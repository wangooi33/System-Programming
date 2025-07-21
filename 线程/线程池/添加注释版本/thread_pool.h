#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

/*
 * 线程池头文件
 * 
 * 提供线程池初始化、添加任务、添加/删除线程、销毁线程池等接口；
 * 使用互斥锁和条件变量实现任务队列的线程安全；
 * 适用于线程数量固定或动态调整的多任务并发场景。
 */

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <errno.h>
#include <pthread.h>

#define MAX_WAITING_TASKS   1000   // 等待队列中最大任务数
#define MAX_ACTIVE_THREADS  20     // 最大活跃线程数（上限保护）

/*
 * 任务结点结构体
 * 单向链表结构，每个结点代表一个待执行的任务
 */
struct task
{
	void *(*do_task)(void *arg); // 任务处理函数指针（线程执行的函数）
	void *arg;                   // 传递给任务函数的参数（可以为 NULL）
	struct task *next;           // 指向下一个任务的指针
};

/*
 * 线程池管理结构体
 */
typedef struct thread_pool
{
	pthread_mutex_t lock;           // 线程池互斥锁，保护任务队列和计数变量
	pthread_cond_t  cond;           // 条件变量，线程挂起/唤醒时使用

	bool shutdown;                  // 标志位：是否正在销毁线程池

	struct task *task_list;         // 任务链表头指针（任务队列）

	pthread_t *tids;                // 动态分配的线程ID数组

	unsigned max_waiting_tasks;     // 等待任务的最大数量（上限）
	unsigned waiting_tasks;         // 当前等待执行的任务数
	unsigned active_threads;        // 当前活跃线程数量（在运行的线程）

} thread_pool;


/*
 * 初始化线程池
 * @param pool               线程池指针（需提前 malloc 分配）
 * @param threads_number     初始线程数（建议小于 MAX_ACTIVE_THREADS）
 * @return                   true 表示成功，false 表示失败
 */
bool init_pool(thread_pool *pool, unsigned int threads_number);

/*
 * 向线程池添加任务
 * @param pool               线程池指针
 * @param do_task            任务函数
 * @param task               任务函数参数
 * @return                   true 表示成功添加任务，false 表示失败（如满队列）
 */
bool add_task(thread_pool *pool, void *(*do_task)(void *arg), void *task);

/*
 * 添加线程至线程池
 * @param pool                   线程池指针
 * @param additional_threads     要添加的线程数
 * @return                       实际成功添加的线程数
 */
int add_thread(thread_pool *pool, unsigned int additional_threads_number);

/*
 * 从线程池中移除线程
 * @param pool                   线程池指针
 * @param removing_threads       要移除的线程数（0表示仅获取当前线程数）
 * @return                       移除后的线程总数
 */
int remove_thread(thread_pool *pool, unsigned int removing_threads_number);

/*
 * 销毁线程池，释放所有资源
 * @param pool                   线程池指针
 * @return                       true 表示成功销毁
 */
bool destroy_pool(thread_pool *pool);

/*
 * 线程函数：线程池中每个线程的主执行逻辑
 * @param arg                    线程池指针
 * @return                       NULL
 */
void *routine(void *arg);

#endif
