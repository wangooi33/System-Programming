/*
 * 【动态优先级】指的是当多个【普通任务】并发运行时，系统会根据其实际运行的表现来动态地调整他们的nice值，任务的表示如下：
 *	A.睡眠时间越多，放着系统资源不用，系统就倾向于判定其为IO消耗性的任务，会逐步提高其优先级
 *	B.睡眠时间越少，拼命抢占系统资源，系统就倾向于判定其为CPU消耗性任务，会逐步降低其优先级
 *
 *	注意：Linux系统中线程的nice值越高，则优先级越低，而nice值越低，反而优先级越高!!!
 *
 * nice值的范围是-20 ~ 19，进程的nice值默认为0。
 * 如果要降低nice值（即想要提高优先级），那么启动程序时必须加sudo，获得管理员的权限才能启动。
 * 
*/


#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

//子线程1  调度策略：OTHER
void *task1(void *arg)
{
	nice(19);
	while(1)
	{
		printf("I am task1,is Running\n");

	}
}

//子线程2  调度策略：OTHER
void *task2(void *arg)
{
	nice(-20);
	while(1)
	{	
		printf("I am task2,is Running\n");
	
	}
}

//主线程  也算是普通任务
int main(int argc, char const *argv[])
{

	//1.创建子线程  普通任务
	pthread_t thread1;
	pthread_create(&thread1,NULL,task1,NULL);

	//2.创建子线程 
	pthread_t thread2;
	pthread_create(&thread2,NULL,task2,NULL);


	pthread_exit(NULL);
	return 0;
}


