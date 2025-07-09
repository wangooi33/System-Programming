//man 3 pthread_


//TID：线程ID

#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

int ts_fd;
int ts_x,ts_y;
void ts_getval(int ts_fd,int *ts_x,int *ts_y)
{
    //清除坐标
    *ts_x=-1;
    *ts_y=-1;

    int cnt=0;
    struct input_event ts_event;
    while(1)
    {
        //read函数默认会阻塞
        read(ts_fd,&ts_event,sizeof(ts_event));
        if(ts_event.type == EV_ABS && ts_event.code == ABS_x)
        {
            *ts_x=ts_event.value;
            cnt++;
        }
        if(ts_event.type == EV_ABS && ts_event.code == ABS_y)
        {
            *ts_y=ts_event.value;
            cnt++;
            if(cnt>=2) break;
        }
    }
}

void *ts_tack(void *arg)
{
    int ts_x,ts_y;//存储触摸屏的坐标

    //如果有参数传递给线程的任务，则需要在任务进行参数类型转换
    int ts_fd = *(int *)arg;

    //线程任务应该是死循环，不会退出
    while(1)
    {
        //获取触摸屏坐标
        ts_getval(ts_fd,&ts_x,&ts_y);
        x=ts_x;
        y=ts_y;
    }
}

int main(int argc.char *argv[])
{
    //1.open
    ts_fd=open("dev/input/event0",O_RDWR);
    if(-1 == ts_fd)
    {
        fprintf (stderr, "open ts error %d, %s\n",
                 errno, strerror (errno));

        exit(1);
    }
    //2.创建子线程，，线程一旦创建则马上开始工作
    pthread_t ts_thread;
    pthread_creat(&ts_thread,NULL,ts_task,(void *)&ts_fd);//&ts_fd 等价于 *ts_fd , 然后进行强转，转换成(void *)

    while(1)
    {
        printf("(%d,%d)\n",ts_x,ts_y);
    }
    return 0;
}