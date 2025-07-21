//*在创建前设置好属性*/
//*线程在终止之后自动释放资源的属性，那就是线程的分离属性。
/******************************************************************************************
*
*		线程的属性设置是有步骤、有顺序的：定义对象 + 初始化对象 + 修改对象 + 使用对象
*
* 		1.创建一个线程属性对象  pthread_attr_t
*   	2.首先需要初始化线程属性对象  pthread_attr_init()
*   	3.可以调用其他的线程属性设置函数修改属性对象中的属性  pthread_attr_set....()
*   	4.线程属性对象中的属性如果都已经设置完成，则可以调用pthread_create()创建线程
*   	
*   	5.如果新线程是使用线程属性对象创建的，并且已经创建成功，则新线程和该对象无关
*   	6.如果线程属性对象没有长期的使用需求，则可以选择调用pthread_attr_destroy()销毁
*   	7.如果一个线程属性对象已经被销毁，则可以选择重新使用pthread_attr_init()初始化
*	
******************************************************************************************/

//设置为分离属性的线程是无法被接合的,线程默认是可连接状态

#include <stdio.h>
#include <pthread.h>


void *task(void *arg)
{
	while(1)
	{
		printf("I am thread,my TID = %d\n",pthread_self());
		pthread_exit(NULL);
	}
}

int main(int argc, char const *argv[])
{
	
	//1.定义线程属性对象
	pthread_attr_t thread_addr;

	//2.初始化线程属性对象
	pthread_attr_init(&thread_addr);

	//3.修改线程属性对象中的分离属性，设置为分离，也就是线程终止时不需要调用pthread_join
	pthread_attr_setdetachstate(&thread_addr,PTHREAD_CREATE_DETACHED);

	//4.利用线程属性对象来创建新线程
	pthread_t thread;
	pthread_create(&thread,&thread_addr,task,NULL);

	//5.主线程的任务
	while(1)
	{

	}

	return 0;
}