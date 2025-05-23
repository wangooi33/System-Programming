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
int main(int argc,char *argv[])
{
    //1.打开有名管道
	int fifo_fd = open("./fifo",O_RDWR);
	if (-1 == fifo_fd)
	{
		fprintf (stderr, "open fifo error %d, %s\n",
                 errno, strerror (errno));

        exit(1);
    }
    //2.进程B从管道文件读取pid
	pid_t pid = 0;
    ssize_t ret = read(fifo_fd, &pid, sizeof(pid));
    if (ret != sizeof(pid)) {
        fprintf (stderr, "read  error %d, %s\n",
                 errno, strerror (errno));

        exit(2);
    }
	//3.关闭有名管道
	close(fifo_fd);

    while(1)
	{
		kill(pid,SIGUSR1);
        kill(pid,SIGUSR2);
		sleep(1);
	}

    return 0;
}