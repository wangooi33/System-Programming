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
#include <signal.h>

//需要把信号SIGINT进行屏蔽
int main(int argc, char const *argv[])
{
	
	//1.创建一个信号集
	sigset_t set;

	//2.把需要屏蔽的信号加入到该集合中
	sigaddset(&set,SIGINT);//ctrl+c

	//3.需要设置该集合的属性  阻塞属性
	sigprocmask(SIG_BLOCK,&set,NULL);
	//解除: sigprocmask(SIG_UNBLOCK, &set, NULL);
	
	while(1);

	return 0;
}
