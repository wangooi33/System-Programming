/*练习：设计一个程序，要求在进程中存在两条线程，
主线程获取键盘的字符串并输入到数组中，
子线程等待主线程输入完成后，在把数组中的字符串输出到终端。
要求使用POSIX信号量实现线程的同步。*/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <strings.h>

//临界资源,应该使用volatile进行修饰，防止编译器对该变量进行优化
volatile char buf[128] = {0};  


//定义一个POSIX匿名信号量,进程中的所有线程都可以访问，所以必须是全局变量
sem_t sem1;


//子线程B的任务,格式是固定的
void * task_B(void *arg)
{
	//线程任务应该是死循环，并且不会退出
	while(1)
	{

		//对匿名信号量进行P操作  sem_wait
		sem_wait(&sem1);

		//对临界资源进行操作
		printf("I am Thread,buf = [%s]\n",buf);
		bzero(buf,sizeof(buf));

		//对匿名信号量进行V操作  sem_post
		sem_post(&sem1);
	}
}

//主线程  A
int main(int argc, char const *argv[])
{	
	//1.对全局的POSIX匿名信号量进行初始化,为了实现同步，需要把信号量的初值设置为0
	sem_init(&sem,0,0);

	//2.创建子线程	
	pthread_t thread_B;
	pthread_create(&thread_B,NULL,task_B,NULL); 


	//3.进入死循环，主线程需要对临界资源进行修改

	//通过键盘获取字符串并输入到数组中
	scanf("%s",buf);

	//对匿名信号量进行V操作  sem_post
	sem_post(&sem1);

	while(1)
	{

	}

	return 0;
}