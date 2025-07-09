//某些线程只是为了获取共享资源的值，而不打算修改共享资源(读写锁)
//读操作可以共享，因此多条线程可以对同一个读写锁加多重读锁。
//写操作天然互斥，因此多条线程中只能有一条线程拥有写锁。


//注意：如果有一条线程准备对资源进行修改，则不应该有其他的线程再对资源进行读或者写，就相当于写操作和读操作是互斥的。
//不能同时进行读写操作
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

//临界资源,应该使用volatile进行修饰，防止编译器对该变量进行优化
volatile int data = 10;  

//读写锁对象，必须是全局变量
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
/*
 * pthread_rwlock_t rwlock;
 * pthread_rwlock_init(&rwlock, NULL);
*/


//子线程B的任务,格式是固定的
void * task_B(void *arg)
{
	//线程任务应该是死循环，并且不会退出
	while(1)
	{
		//获取读操作的锁
		pthread_rwlock_rdlock(&rwlock);

		//对临界资源进行读操作
		printf("I am Thread_B,data = %d\n",data);
		sleep(1);

		//释放读操作的锁
		pthread_rwlock_unlock(&rwlock);	
	}
}

//子线程C的任务,格式是固定的
void * task_C(void *arg)
{
	//线程任务应该是死循环，并且不会退出
	while(1)
	{
		//获取读操作的锁
		pthread_rwlock_rdlock(&rwlock);

		//对临界资源进行读操作
		printf("I am Thread_C,data = %d\n",data);
		sleep(1);

		//释放读操作的锁
		pthread_rwlock_unlock(&rwlock);		
	}
}

//主线程  A
int main(int argc, char const *argv[])
{	
	//1.对创建的读写锁对象进行初始化
	pthread_rwlock_init(&rwlock,NULL);

	//2.创建子线程	
	pthread_t thread_B;
	pthread_t thread_C;

	pthread_create(&thread_B,NULL,task_B,NULL); 
	pthread_create(&thread_C,NULL,task_C,NULL); 

	//3.进入死循环，主线程需要对临界资源进行修改
	while(1)
	{
		//主线程会阻塞等待，10s会解除阻塞
		sleep(10);

		//获取写操作的锁
		pthread_rwlock_wrlock(&rwlock);

		//对临界资源进行读操作
		data += 20;
		printf("I am main_Thread,data = %d\n",data);
		sleep(5);

		//释放写操作的锁
		pthread_rwlock_unlock(&rwlock);	
	}

	return 0;
}