#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
int main()
{
	//ipcrm -m <shm_id>         # 删除共享内存段
	//ipcrm -q <msg_id>         # 删除消息队列
	//ipcrm -s <sem_id>         # 删除信号量集合

	int msgctl(int msqid, int cmd, struct msqid_ds *buf);

}