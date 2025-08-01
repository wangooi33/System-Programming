/*
 * 由于多个进程都会对这段共享内存进行映射，并且映射之后这些进程可能会同时对共享内存进行读写访问，
 * 所以就会导致存储在共享内存中的数据被其他进程修改，造成进程间的异常，
 * 所以使用信号量来实现进程间资源协同的目的。
*/

//被进程或者线程共同访问资源称为临界资源（Critical Resources），
//而程序中访问这些临界资源的代码段被称为临界区（Critical Zone）。

#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <string.h>
#include <errno.h>

    ////1.打开信号量集，如果不存在则创建，如果存在则打开
	// int sem_id = semget(ftok(".",20),1,IPC_CREAT|IPC_EXCL|0644);
	// if (-1 == sem_id)
	// {
	// 	fprintf(stderr, "semget error,errno:%d,%s\n",errno,strerror(errno));
	// 	sem_id = semget(ftok(".",20),1,0644);
	// }

	
// 信号量初始化值
#define SEM_INIT_VALUE 1
// 自定义union semun（POSIX要求用户自行定义）
union semun 
{
    int val;                  // 用于SETVAL:设置某个信号量的初始值
    struct semid_ds *buf;     // 用于IPC_STAT, IPC_SET:获取信号量集合的状态或者设置权限
    unsigned short *array;    // 用于GETALL, SETALL:批量获取/设置信号量集合中所有信号量的值
};
int main(int argc,char *argv[]) 
{
    // 1. 生成key
    key_t key = ftok(".", 20);
    if (key == -1) {
        fprintf(stderr, "ftok error %d, %s\n", errno, strerror(errno));
        exit(1);
    }

    // 2. 创建一个包含1个信号量（信号量元素数量）的信号量集
    int semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666);
    if (semid == -1) {
        fprintf(stderr, "semget error %d, %s\n", errno, strerror(errno));
        exit(2);
    }

    // 3. 初始化信号量的值为1
    union semun sem_val;
    sem_val.val = SEM_INIT_VALUE;
    if (semctl(semid, 0, SETVAL, sem_val) == -1) {
        fprintf(stderr, "semctl SETVAL error %d, %s\n", errno, strerror(errno));
        exit(3);
    }

	//4.获取信号量当前的值
	int val = semctl(semid, 0, GETVAL);
	printf("当前信号量值：%d\n", val);

    printf("信号量ID:%d,信号量初值= %d\n", semid, SEM_INIT_VALUE);
    return 0;

}