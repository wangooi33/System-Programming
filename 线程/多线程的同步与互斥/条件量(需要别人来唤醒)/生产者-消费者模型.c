#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

int ready = 0;
pthread_mutex_t lock;//互斥锁
pthread_cond_t cond;//条件量

void* consumer(void* arg) 
{
    pthread_mutex_lock(&lock);
    while (ready == 0) 
	{
        printf("消费者等待中...\n");
        pthread_cond_wait(&cond, &lock);  // 自动解锁并等待
    }
    printf("消费者收到信号，开始消费！\n");
    pthread_mutex_unlock(&lock);
    return NULL;
}

void* producer(void* arg) 
{
    sleep(1); // 模拟生产时间
    pthread_mutex_lock(&lock);
    ready = 1;
    printf("生产者生产完毕，发送信号！\n");
    pthread_cond_signal(&cond);  // 唤醒消费者
    pthread_mutex_unlock(&lock);
    return NULL;
}

int main() 
{
    pthread_t c, p;

    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_create(&c, NULL, consumer, NULL);
    pthread_create(&p, NULL, producer, NULL);

    pthread_join(c, NULL);
    pthread_join(p, NULL);

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);
    return 0;
}
