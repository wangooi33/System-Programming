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


int main(int argc, char const *argv[])
{
	//1.进程A创建一个有名管道
	int ret = mkfifo(FIFO_PATH,0777);
	if (ret == -1)
	{
		fprintf(stderr, "mkfifo error,errno:%d,%s\n",
						errno,strerror(errno));
		
		exit(1);
	}

	//2.进程A打开有名管道
	int fifo_fd = open(FIFO_PATH,O_RDWR);
	if (-1 == fifo_fd)
	{
		fprintf(stderr, "open error,errno:%d,%s\n",
						errno,strerror(errno));
		
		exit(2);
	}

	//3.进程A获取系统时间并转换为格式化字符串，写入到管道文件
	char *ptime;
	while(1)
	{
		ptime = ctime(time(NULL));
		write(fifo_fd,ptime,strlen(ptime));
		sleep(1);
	}
	
	//4.关闭有名管道
	close(fifo_fd);

	return 0;
}