#include <stdio.h>
#include <pthread.h>

voi *task_1(void *arg)
{
    while(1)
    {
        pthread_exit();//子线程调用，则子线程结束
    }
}

void *task_2(void *arg)
{
    while(1)
    {
        
    }
}
int main(int argc.char *argv[])
{
    //1.创建子线程，，线程一旦创建则马上开始工作
    pthread_t thread_1;
    pthread_creat(&thread_1,NULL,task_1,NULL);

    pthread_t thread_2;
    pthread_creat(&thread_2,NULL,task_2,NULL);

    while(1)
    {
        pthread_exit();//主线程可以优先于子线程结束
    }
    return 0;
}