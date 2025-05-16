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
	//1.创建一条匿名管道  pipe   pipefd[0] 读操作   pipefd[1]  写操作
	int pipefd[2] ={0};

	int ret = pipe(pipefd);
	if (ret == -1)
	{
		fprintf(stderr, "pipe error,errno:%d,%s\n",errno,strerror(errno));
		exit(1); //exit函数可以终止进程，并把进程的终止状态提供给该进程的父进程
	}

	//2.此时可以创建子进程  fork  子进程会拷贝父进程的数据段、代码段、堆栈段
	int child_pid = fork();

	//3.分析当前的进程空间是父进程 or 子进程 or 创建失败
	if (child_pid > 0)
	{
		//说明是父进程，父进程需要从匿名管道中读取数据
		char recvbuf[128] ={0};

		read(pipefd[0],recvbuf,sizeof(recvbuf)); //父进程会阻塞
		printf("my is parent,read from pipe data is [%s]\n",recvbuf);	

		wait(NULL); //父进程需要回收子进程的资源，也会阻塞
	}
	else if(child_pid == 0)
	{
		//说明是子进程，子进程向匿名管道中写入字符串
		char sendbuf[128] = "my is child,hello parent";
		write(pipefd[1],sendbuf,strlen(sendbuf));
	}
	else
	{
		fprintf(stderr, "fork error,errno:%d,%s\n",errno,strerror(errno));
		exit(2); //exit函数可以终止进程，并把进程的终止状态提供给该进程的父进程
	}

	return 0;
}