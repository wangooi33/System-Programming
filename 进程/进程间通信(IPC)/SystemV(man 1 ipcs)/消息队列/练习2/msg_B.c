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
	//1.打开一个消息队列
	key_t key  = ftok(".", 0xFFFFFF01);
	int msg_id = msgget(key,0644);
	if (msg_id == -1)
	{
		fprintf(stderr, "msgget error,errno:%d,%s\n", errno,strerror(errno));
	}

	/*
	 * 如果要接收2个字节，但实际上有4个字节，
	 * 如果第3个参数不填MSG_NOERROR则不接收，
	 * 填了的话只接收2字节
	*/
	//2.从消息队列接收消息
	struct msgbuf first;
	bzero(&first,sizeof(first));

	msgrcv(msg_id,&first,4,0,0); //默认阻塞
	/*
	 *第四个参数：msgtyp指的是要接收消息的类型，在调用msgsnd函数时构造的消息结构体中有该成员的值。
	 * 1)等于0：指的是不区分类型，直接读取MSG中的第一个消息。
	 * 2)大于0：读取类型为指定msgtyp的第一个消息（若msgflg被配置了MSG_EXCEPT则读取除了类型为msgtyp的第一个消息）。
	 * 3)小于0：读取类型小于等于msgtyp绝对值的第一个具有最小类型的消息。例如当MSG对象中有类型为3、1、5类型消息若干条，当msgtyp为-3时，类型为3的第一个消息将被读取。
	 *
	*/

	printf("mtype = %d\n",first.mtype); //1
	printf("mtext = %d\n",first.mtext); // pid不会是3649，应该比3649小

	// msgrcv(msg_id,&first,4,0,0); //默认阻塞

	// printf("mtype = %d\n",first.mtype); //1
	// printf("mtext = %d\n",first.mtext); // pid不会是3649，应该比3649小
	
	return 0;
}