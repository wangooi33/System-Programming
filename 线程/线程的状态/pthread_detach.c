
#include <stdio.h>
#include <pthread.h>

//可以选择在线程的任务重调用pthread_detach来强制设置该线程的属性为分离属性
void *task(void *arg)
{
	//可以设置线程的属性为分离属性，此时线程终止时就不需要其他线程调用pthread_join函数
	pthread_detach(pthread_self());

	while(1)
	{
		printf("I am thread,my TID = %d\n",pthread_self());
		pthread_exit(NULL); //当线程终止时，线程的资源会自动的释放
	}
}

int main(int argc, char const *argv[])
{
	
	//1.创建新线程,线程默认是可结合的，所以当线程终止时，需要其他线程调用pthread_join函数
	pthread_t thread;
	pthread_create(&thread,NULL,task,NULL);

	//5.主线程的任务
	while(1)
	{

	}

	return 0;
}