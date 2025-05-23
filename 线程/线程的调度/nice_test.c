
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


