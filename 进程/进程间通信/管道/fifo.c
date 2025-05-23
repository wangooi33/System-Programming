#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

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