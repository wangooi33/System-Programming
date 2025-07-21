/*
 *注意：消息队列默认的属性是阻塞的，
 *也就是当待写入的消息的长度大于消息队列剩余空间时，默认阻塞，(可修改)
 *直到消息队列的容量足够容纳时会解除阻塞。
*/

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
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

//用户可以构造消息的结构  struct msgbuf
struct msgbuf
{
	long mtype;  //消息类型，必须大于0
	int  mtext;  //消息正文
};

int main(int argc, char const *argv[])
{
	//1.创建一个消息队列
	key_t key  = ftok(".", 0xFFFFFF01); //proj_id只使用低8bit  0x01
	int msg_id = msgget(key,IPC_CREAT|0644);
	
	if (msg_id == -1)
	{
		fprintf(stderr, "msgget error,errno:%d,%s\n", errno,strerror(errno));
	}

	//2.向消息队列发送消息
	struct msgbuf first;
	first.mtype = 1;
	first.mtext = getpid();

	msgsnd(msg_id,&first,4,0); //默认阻塞,将第4个参数设置为IPC_NOWAIT则表示不阻塞，若不阻塞，则直接返回并报错

	while(1);

	return 0;
}