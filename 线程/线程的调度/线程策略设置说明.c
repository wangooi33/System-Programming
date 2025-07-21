/*
 * 【静态优先级】指的是任务一旦设置好优先级之后就不能再改变，
 * 相当于是任务本身的属性，通过Linux系统内核源码中的头文件sched.h可以知道，
 * 线程的优先级范围是0~99，优先级数值越大则优先级越高。
 * 
 * *******************************************************************
 * 
 * 【SCHED_OTHER】
 * SCHED_OTHER调度策略指的是分时调度策略，是Linux系统中默认的调度策略，
 * 一般采用CFS（Completely Fair Scheduler）算法，该算法为运行队列中的每一个进程都设置一个虚拟时钟vruntime（Virtual Runtime）。
 * 如果一个进程得到执行，随着执行时间的不断增长，其vruntime也将不断增大，
 * 没有得到执行的进程vruntime将保持不变。而调度器将会选择最小的vruntime那个进程来执行。
 * 这就是所谓的“完全公平”。不同优先级的进程其vruntime增长速度不同，优先级高的进程vruntime增长得慢，所以可能会得到更多的运行机会。
 *
 * 【注意】：如果一个线程以SCHED_OTHER调度策略进行创建，线程的静态优先级必须设置为0。
 * 
 *
 * 【SCHED_FIFO】
 * SCHED_FIFO调度策略指的是实时调度策略，采用“先到先服务”的调度方式，当一条线程采用该调度策略则会一直运行，
 * 直到它被更高优先级的线程抢占或者主动放弃CPU，才会交出控制权。
 * 当线程完成后，内核会去寻找处于就绪状态相同优先级的线程，如果不存在，则寻找低优先级线程。
 * 该调度策略实现了数据的互斥，在线程运行的时间内其他相同优先级线程无法进行资源抢占。
 * 
 * 
 * 【SCHED_RR】
 * SCHED_RR（Round Robin，中文意为轮询）指的是实时调度策略，是一种基于时间片轮转的调度策略，
 * 它会给每个线程设置一个固定的优先级，并按照优先级顺序对线程进行轮流调度。
 * 当一条线程采用该调度策略则会一直运行，直到它被更高优先级的线程抢占、主动放弃CPU以及消耗完自己的时间片，才会交出控制权。
 * 时间片是线程运行的最小时间单元，由操作系统预先设定。
 * 当时间片用完时，该线程自动交出控制权，之后内核会按照和FIFO相同的方式搜索下一个工作线程。
 * 轮转调度可以防止某一个任务连续占用太多的资源，而导致其他线程信息得不到及时处理。
 * 缺点是轮转调度会增大由于任务切换（任务上下文）而导致的开销。
 * 
 * 如果线程的调度策略是SCHED_FIFO或者SCHED_RR，则线程的优先级可以设置为1~99，表示系统任务，
 * 如果是线程的优先级是1~99，则运行程序时必须获取超级用户(root)权限，比如 sudo  ./xxx。
 * 
 * ****************************************************************************************************************
 * 
 * 
*/


#include <stdio.h>
#include <pthread.h>


pthread_attr_t attr;


//子线程1  调度策略：FIFO
void *task1(void *arg)
{
	
	while(1)
	{
		printf("I am task1,is Running\n");
		sleep(1);
	}
}

//子线程2  调度策略：FIFO
void *task2(void *arg)
{
	
	while(1)
	{	
		printf("I am task2,is Running\n");
		sleep(5);
	}
}

//主线程  也算是普通任务
int main(int argc, char const *argv[])
{
	struct sched_param param; 

	//1.对线程属性对象进行初始化
	pthread_attr_init(&attr);//调用pthread_attr_init()函数初始化之后，默认是继承创建该线程的线程的调度属性。

	//2.初始化之后才可以获取线程属性对象中的继承调度属性
	int inheritsched;

	pthread_attr_getinheritsched(&attr,&inheritsched);//设置以及获取线程的继承调度属性

	//表达式成立，说明默认的线程属性对象中的继承调度属性的值：继承
	if (inheritsched == PTHREAD_INHERIT_SCHED)
	{
		printf("INHERIT\n");
	}
	else
	{
		printf("EXPLICIT\n");
	}
	//3.修改线程属性对象中的继承调度属性：不继承
	pthread_attr_setinheritsched(&attr,PTHREAD_EXPLICIT_SCHED);

	//4.修改线程属性对象的调度策略为：先到先服务  SCHED_FIFO
	pthread_attr_setschedpolicy(&attr,SCHED_FIFO);

	//5.修改创建的子线程的静态优先级
	param.sched_priority = 1;
	pthread_attr_setschedparam(&attr,&param);

	//6.创建子线程 子线程1的优先级是1
	pthread_t thread1;
	pthread_create(&thread1,&attr,task1,NULL);

	//7.再次修改创建的子线程的静态优先级
	param.sched_priority = 2;
	pthread_attr_setschedparam(&attr,&param);

	//8.创建子线程 子线程1的优先级是2
	pthread_t thread2;
	pthread_create(&thread2,&attr,task2,NULL);


	pthread_exit(NULL);
	return 0;
}


