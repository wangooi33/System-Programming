/*
 *不要在同一个线程中对已经上锁的互斥量重新上锁，
 *这样会导致死锁出现，应该避免该情况出现，
 *上锁之后应该在使用完资源后及时进行解锁。
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * 动态初始化：（可以指定锁的属性）
 * pthread_mutex_t lock;
 * pthread_mutex_init(&lock, NULL);  // 第二个参数为属性，NULL 表示默认属性
 *
 * 静态初始化：（适合：全局变量、局部 static 变量）
 * pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
 * 
*/

int x;

pthread_mutex_t lock;//定义变量

void *task(void *arg)
{
	while(1)
	{
		pthread_mutex_lock(&lock); 	//上锁

		x = 10;

		pthread_mutex_unlock(&lock); //解锁
	}
}

int main(int argc, char const *argv[])
{
	pthread_mutex_init(&lock, NULL);
	while(1)
	{
		pthread_mutex_lock(&lock); 	//上锁
		
		x = 20;

		pthread_mutex_unlock(&lock); //解锁
	}
	return 0;
}