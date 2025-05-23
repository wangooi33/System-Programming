#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>
#include <errno.h>

// struct sembuf{
//     unsigned short sem_num;  /* semaphore number */
//     short          sem_op;   /* semaphore operation */
//     short          sem_flg;  /* operation flags */
//  };
int main(int argc, char const *argv[])
{
    //0.打开信号量集，如果不存在则创建，如果存在则打开
	int sem_id = semget(ftok(".",20),1,IPC_CREAT|IPC_EXCL|0644);
	if (-1 == sem_id)
	{
		fprintf(stderr, "semget error,errno:%d,%s\n",errno,strerror(errno));
		sem_id = semget(ftok(".",20),1,0644);
	}

	//1.打开共享内存段，如果不存在则创建，如果存在则打开
	int shm_id = shmget(ftok(".",10),4,IPC_CREAT|IPC_EXCL|0644);
	if (-1 == shm_id)
	{
		fprintf(stderr, "shmget error,errno:%d,%s\n",errno,strerror(errno));
		shm_id = shmget(ftok(".",10),4,0644);
	}

	//2.把共享内存段连接到进程空间  shmat()  默认是可读可写  默认会初始化为0
	int *data = (int *)shmat(shm_id,NULL,0);
	if (data == (void *)-1)
	{
		fprintf(stderr, "shmat error,errno:%d,%s\n",errno,strerror(errno));
		return -1;
	}
    *data = 0;  // 写共享内存

    //3.PV操作
    struct sembuf p = {0, -1, 0}; // P操作
    struct sembuf v = {0, +1, 0}; // V操作

    //4.
    while(1)
    {
        semop(sem_id, &p, 1);   // P 操作
        (*data)++;
        printf("B: data = %d\n", *data);
        semop(sem_id, &v, 1);   // V 操作
        sleep(1);
    }

    //shmdt(data);
    return 0;
}