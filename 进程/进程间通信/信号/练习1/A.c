#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>

/*
 *Linux系统中的信号编号为1~64，
 *其中编号为1~31的信号为普通信号，指的是当进程接收到了很多的信号请求但是又不能及时处理时，把其余未被处理的信号直接丢弃，只留下一个信号。
 *编号为34~64的信号为实时信号，，指的是当进程接收到了很多信号请求但是又不能及时处理时，会把未处理的信号形成队列，然后按照顺序依次处理。
 *
*/

//信号的响应接口，该函数不需要用户手动调用
void siganl_handler(int signum)
{
    switch (signum)
    {
    case SIGUSR1:
                printf("hello\n");break;
    case SIGUSR2:
                printf("world\n");break;
    }
}
int main(int argc,char *argv[])
{
    //0.创建一个有名管道
	int ret = mkfifo("./fifo",0664);
	if (ret == -1)
	{
		fprintf (stderr, "mkfifo error %d, %s\n",
                 errno, strerror (errno));

        exit(1);
	}
	//1.打开有名管道
	int fifo_fd = open("./fifo2",O_RDWR);
	if (-1 == fifo_fd)
	{
		fprintf (stderr, "open fifo error %d, %s\n",
                 errno, strerror (errno));

        exit(2);
	}
	//2.向有名管道写入数据
	pid_t pid = getpid();
    ret = write(fifo_fd, &pid, sizeof(pid_t));
    //write() 为什么能写 int？	
    //    因为内存中任何类型最终都是字节序列（char 数组）
	if(ret != sizeof(pid_t))
	{
		fprintf (stderr, "write error %d, %s\n",
                 errno, strerror (errno));

        exit(3);
	}
	//3.关闭有名管道
	close(fifo_fd);
    //4.设置信号的相应接口，也就是当收到信号时需要执行的操作
    signal(SIGUSR1,siganl_handler);
    signal(SIGUSR2,siganl_handler);

    while(1);

    return 0;
}