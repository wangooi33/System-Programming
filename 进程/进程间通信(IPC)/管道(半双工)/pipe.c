//只适合在有亲缘关系的进程中使用
//注意：匿名管道虽然可以在具有亲缘关系的进程中使用，
//但是匿名管道是不保证数据的原子性。
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
 *用户把对数据写入到管道之后，数据是会在内核的缓冲区中进行暂存的，直到从管道中读取该数据为止。
 *
 *注意：内核的缓冲区大小是固定的（linux系统下默认是4M大小），
 *如果写入的速度快于读取的速度，则可能会发生缓冲区满的情况，
 *如果读取的速度快于写入的速度，
 *如果管道内部没有数据，则读取数据会出现阻塞等待的情况。
 *
 *所谓的阻塞实际上就是系统将该进程挂起，等待资源就绪再继续调度的一种状态，
 *这种阻塞的状态有利于系统中别的进程可高效地使用闲置CPU资源，
 *提高系统的吞吐量。
 *
 *
 *如果父进程中创建了子进程，子进程会把父进程的代码段、数据段、堆栈段等完全复制一份，
 *所以应该在复刻进程之前创建匿名管道，这样子进程就可以把匿名管道的文件描述符复制到自己的进程空间中，
 *这样父进程和子进程就可以对管道进行读写访问。
 *
*/

int main(int argc, char const *argv[])
{
	//1.创建一条匿名管道  pipe   pipefd[0] 读操作   pipefd[1] 写操作
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