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

//Linux系统下shell命令：kill  （查看：kill -l）

/*
 * 信号的产生:
 *		1.按键触发:按下快捷键，内核发送指定信号给进程，例如Ctrl+C
 * 		  按键触发指的是用户按下某个快捷键，然后由内核发送指定的信号给进程，
 *		  比如用户准备在Linux系统的终端输入shell命令，则会先开启一个Terminal终端，
 *		  然后在终端中执行了一个可执行文件，
 *		  此时操作系统会创建一个进程，并把可执行文件的代码段和数据段加载到进程空间中，
 *		  并分配CPU时间片给到该进程，此时进程会从就绪态进入运行态。
 * 
 * 		  但是在进程执行过程中用户打算提前结束进程，所以用户在终端按下快捷键Ctrl+C，
 * 		  键盘就产生了一个硬件中断，操作系统会把Ctrl+C解释为SIGINT信号并记录在进程的PCB中。
 * 		  此时CPU会暂停执行用户空间中的代码，然后去执行内核空间中的硬件中断，
 * 		  当硬件中断执行完成后CPU会返回用户空间，但是CPU需要先处理记录在进程PCB中的信号，
 * 		  而CPU发现此时进程PCB中的信号是SIGINT，该信号含义是终止进程，
 * 		  所以进程会切换到终止态，相当于结束进程。
 *
 * 		2.硬件异常:硬件异常产生的信号会由系统硬件进行检测，比如进程中执行除以0的指令会导致ALU异常，
 * 		  或者进程中访问了非法内存地址会导致MMU异常，此时内核会发送给进程相关的信号。
 * 
 * 		3.调用接口:kill()     man 2 kill
 * 
 * 		4.发送指令:shell命令，kill 		man 1 kill
 * 
 * 		5.内核检测:当内核检测到某种软件条件发生时也可以通过信号通知进程，
 * 		  例如内核检测到闹钟超时则会产生SIGALRM信号，
 * 		  或者当内核检测到进程向读端已关闭的管道写数据时就产生SIGPIPE信号。
 * 		  Linux系统下提供了一个叫做alarm()的函数接口，alarm翻译为中文是闹钟的意思，
 * 		  也就是说该函数可以设置内核定时器的时间，时间是以秒为单位的，
 * 		  当设置的秒数到达时，就相当于闹钟时间达到，
 * 		  此时内核检测到闹钟到达之后就会向调用该函数的进程发送SIGALRM，
 * 		  该信号的默认含义是终止进程。
 * 
*/



/*
 *Linux系统中的信号编号为1~64，
 *其中编号为1~31的信号为普通信号，指的是当进程接收到了很多的信号请求但是又不能及时处理时，把其余未被处理的信号直接丢弃，只留下一个信号。
 *编号为34~64的信号为实时信号，，指的是当进程接收到了很多信号请求但是又不能及时处理时，会把未处理的信号形成队列，然后按照顺序依次处理。
 *
*/

//man 7 signal ，信号的处理：默认、捕捉、忽略

//信号的响应接口，该函数不需要用户手动调用
void signal_handler(int signum)
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
    //write() 为什么能写 int？	因为内存中任何类型最终都是字节序列（char 数组）
	if(ret != sizeof(pid_t))
	{
		fprintf (stderr, "write error %d, %s\n",
                 errno, strerror (errno));

        exit(3);
	}
	//3.关闭有名管道
	close(fifo_fd);
    //4.设置信号的相应接口，也就是当收到信号时需要执行的操作
	/*
	 *忽略:signal(SIGUSR1,SIG_IGN);
	 *默认:signal(SIGUSR1,SIG_DFL);
	*/
    signal(SIGUSR1,siganl_handler);
    signal(SIGUSR2,siganl_handler);

    while(1);

    return 0;
}