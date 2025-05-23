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
	pthread_attr_init(&attr);

	//2.初始化之后才可以获取线程属性对象中的继承调度属性
	int inheritsched;

	pthread_attr_getinheritsched(&attr,&inheritsched);

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


