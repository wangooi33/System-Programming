/*
 * 可以传输指定类型(用户可以自行定义)的数据，相同类型的数据根据到达顺序在队列中进行排队。
 *
 * 不同类型的数据不能处于同一个队列中，也就是说系统中可能存在多个消息队列，
 * 每个消息队列中的数据类型都是不同的，所以用户打算读取消息队列中的数据时也需要指定数据类型，
 * 才可以从存储该类型数据的消息队列中读取有效数据。
 * 
*/

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