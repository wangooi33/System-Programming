#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>
#include <sys/shm.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <unistd.h>



#define SHM_SIZE  4

int main(int argc, char const *argv[])
{
	
	//1.打开一个共享内存，如果不存在则创建
	int shm_id = shmget(ftok(".",50),SHM_SIZE,IPC_CREAT|IPC_EXCL|0644);
	
	
	if (shm_id == -1)
	{
		if(errno == EEXIST)
		{
			//此时可以再次调用该函数打开共享内存
			shm_id = shmget(ftok(".",50),4,0644);
		}
		else
		{
			fprintf(stderr, "shmget error,errno:%d,%s\n",errno,strerror(errno));
			exit(1);
		}
	}

	//2.把打开的共享内存段映射到自己的进程空间中
	int * pshm = (int *)shmat(shm_id,NULL,0);

	//3.打开一个POSIX有名信号量，如果不存在则创建
	sem_t * psem = sem_open("named_sem", O_CREAT|O_EXCL,0644,0);

	if(psem == SEM_FAILED)
	{
		if(errno == EEXIST)
		{
			//此时可以再次调用该函数打开共享内存
			psem = sem_open("named_sem",0);
		}
		else
		{
			fprintf(stderr, "sem_open error,errno:%d,%s\n",errno,strerror(errno));
			exit(2);
		}
	}

	//4.进程A先对临界资源进行访问，然后对POSIX信号量进行V操作
	
	*pshm = getpid();

	sem_post(psem);
	

	return 0;
}