/*
 * pthread_kill()的函数接口，用户利用该接口可以向指定的线程发送信号。
 *
 * 如果用户打算给某条线程发送信号的同时还想要发送一个额外的数据，Linux系统提供了一个名称叫做pthread_sigqueue()的函数接口可以实现该功能。
 * 
*/


#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

sigset_t set;
pthread_t thread;

//信号响应接口
void signal_handler(int signum)
{
	printf("I am taskA,My TID  = %d\n",thread);
}


//子线程A  调度策略：OTHER
void *task1(void *arg)
{
	thread = pthread_self();
	
	while(1)
	{
		

	}
}

//子线程B  调度策略：OTHER
void *task2(void *arg)
{
	sigaddset(&set,SIGINT);
	sigprocmask(SIG_BLOCK,&set,NULL);//信号的屏蔽

	while(1)
	{	
		
	
	}
}

//子线程C  调度策略：OTHER
void *task3(void *arg)
{
	sigaddset(&set,SIGINT);
	sigprocmask(SIG_BLOCK,&set,NULL);

	while(1)
	{	
		
	
	}
}

int main(int argc, char const *argv[])
{
	signal(SIGINT,signal_handler);

	//1.创建子线程 
	pthread_t thread1;
	pthread_create(&thread1,NULL,task1,NULL);

	//2.创建子线程 
	pthread_t thread2;
	pthread_create(&thread2,NULL,task2,NULL);

	//2.创建子线程 
	pthread_t thread3;
	pthread_create(&thread3,NULL,task3,NULL);

	pthread_exit(NULL);

	return 0;
}