/*********************************************************************************
*
* 	申请共享内存段，系统分配的是物理内存，所以共享内存段使用的物理地址，但是进程
* 	的进程空间中需要使用虚拟地址，所以需要把申请的共享内存映射到进程空间，这样MMU
* 	就会把物理地址转换为虚拟地址，进程通过虚拟地址访问物理内存。
* 	
* ******************************************************************************/
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
#include <sys/ipc.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(int argc, char const *argv[])
{
	//1.打开共享内存段，如果不存在则创建，如果存在则打开
	int shm_id = shmget(ftok(".",10),256,IPC_CREAT|IPC_EXCL|0644);
	if (-1 == shm_id)
	{
		fprintf(stderr, "shmget error,errno:%d,%s\n",errno,strerror(errno));
		shm_id = shmget(ftok(".",10),256,0644);
	}

	//2.把共享内存段连接到进程空间  shmat()  默认是可读可写  默认会初始化为0
	char *pshm = (char *)shmat(shm_id,NULL,0);
	if (pshm == (void *)-1)
	{
		fprintf(stderr, "shmat error,errno:%d,%s\n",errno,strerror(errno));
		return -1;
	}
		
	//3.向共享内存中写入数据  
	sprintf(pshm,"I am ProcessA,My PID = %d",getpid());

	while(1);

	return 0;
}