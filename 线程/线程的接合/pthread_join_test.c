//其他线程可使用 pthread_join() 来释放僵尸线程的资源，并可获得其退出时返回的退出值

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

pthread_t main_thread; //存储主线程ID

//线程的任务
void * task(void *arg)
{
	//用于存储主线程的终止状态
	int *exit_status;

	while(1)
	{
		//子线程等待主线程终止并输出主线程的终止状态，如果等待接合的线程未终止，则该函数会阻塞
		pthread_join(main_thread,(void **)&exit_status);
		printf("main thread exit status code = %d\n",*exit_status);
		pthread_exit(NULL);
	}
}



//主线程
int main(int argc, char const *argv[])
{

	//1.创建子线程，线程一旦创建成功，则马上开始工作	
	pthread_t thread;
	pthread_create(&thread,NULL,task,NULL); 

	//2.获取自身的线程ID
	main_thread = pthread_self();

	//3.主线程终止
	int exit_status = 100;

	sleep(5);

	pthread_exit((void *)&exit_status); //主线程可以优先于子线程结束
	
	return 0;
}

