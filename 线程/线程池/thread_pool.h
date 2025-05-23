#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include <errno.h>
#include <pthread.h>

#define MAX_WAITING_TASKS	1000   //处于等待状态的线程数量最大为1000
#define MAX_ACTIVE_THREADS	20     //活跃的线程数量 

//任务结点  单向链表的节点，类型
struct task
{
	void *(*do_task)(void *arg); //任务函数指针  指向线程要执行的任务  格式是固定的
	void *arg;					 //需要传递给任务的参数，如果不需要，则NULL

	struct task *next;			 //指向下一个任务结点的指针
};


//线程池的管理结构体
typedef struct thread_pool
{
	pthread_mutex_t lock;		// 互斥锁
	pthread_cond_t  cond;		// 条件量

	bool shutdown;				//是否需要销毁线程池

	struct task *task_list;		//用于存储任务的链表

	pthread_t *tids;			//用于记录线程池中线程的ID

	unsigned max_waiting_tasks;	//线程池中线程的数量最大值
	unsigned waiting_tasks;		//处于等待状态的线程数量
	unsigned active_threads;	//正在活跃的线程数量
}thread_pool;


//初始化线程池
bool init_pool(thread_pool *pool, unsigned int threads_number);

//向线程池中添加任务
bool add_task(thread_pool *pool, void *(*do_task)(void *arg), void *task);

//先线程池中添加线程
int  add_thread(thread_pool *pool, unsigned int additional_threads_number);

//从线程池中删除线程
int  remove_thread(thread_pool *pool, unsigned int removing_threads_number);

//销毁线程池
bool destroy_pool(thread_pool *pool);

//任务函数
void *routine(void *arg);


#endif
