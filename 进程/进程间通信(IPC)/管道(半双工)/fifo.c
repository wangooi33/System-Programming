//打开一个用于读取的FIFO通常会阻塞，直到其他进程打开相同的FIFO进行写入
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*
 *命名管道（FIFO）是 内核中临时创建的内存缓冲区，它的特点是：
 *它不具有持久性：一旦两端（写端和读端）都关闭，内核会清除其中的所有缓存数据；
 *
*/

int main(int argc, char const *argv[])
{

	//0.创建一个有名管道
	int ret = mkfifo("./fifo2",0664);
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
	ret = write(fifo_fd,"hello world",11);
	if(ret != 11)
	{
		fprintf (stderr, "write error %d, %s\n",
                 errno, strerror (errno));

        exit(3);
	}

	//3.关闭有名管道
	close(fifo_fd);

	return 0;
}