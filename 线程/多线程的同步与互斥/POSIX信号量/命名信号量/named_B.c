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

	//4.死循环，防止进程终止
	while(1)
	{
		//进程B必须等待进程A把PID写入到共享内存中之后才可以输出共享内存段中的数据
		
		//进程B需要进行P操作
		sem_wait(psem);

		//访问临界资源
		printf("shared memory data is [%d]\n",*pshm);
	}
	
	return 0;
}