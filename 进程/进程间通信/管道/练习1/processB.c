#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#define FIFO_PATH	"/tmp/fifo"
#define LOG_PATH	"./log.txt"

int main(int argc, char const *argv[])
{
	//1.进程B创建一个log.txt
	int log_fd = open(LOG_PATH,O_RDWR|O_CREAT);
	if (-1 == log_fd)
	{
		fprintf(stderr, "open log.txt error,errno:%d,%s\n",
						errno,strerror(errno));
		
		exit(1);
	}

	//2.进程B打开有名管道
	int fifo_fd = open(FIFO_PATH,O_RDWR);
	if (-1 == fifo_fd)
	{
		fprintf(stderr, "open fifo error,errno:%d,%s\n",
						errno,strerror(errno));
		
		exit(2);
	}

	//3.进程B从管道文件读取时间并写入到log.txt
	char timebuf[256] = {0};
	while(1)
	{
		
		read(fifo_fd,timebuf,sizeof(timebuf));
		write(log_fd,timebuf,sizeof(timebuf));
		
		bzero(timebuf,sizeof(timebuf));

		sleep(1);
	}
	
	//4.关闭有名管道
	close(fifo_fd);
	close(log_fd);
	return 0;
}