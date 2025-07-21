#include <stdio.h>

//shell命令创建，ipcmk -Q		man 1 ipcmk

int main(int argc,char *argv[])
{
    //1.创建一个消息队列
	//key：组成句柄的必要材料
    key_t key = ftok(".",0xFFFFFF01);//proj_id只使用低8bit  0x01，必须是非0
    int msg_id = msgget(key,IPC_CREAT);
    if(msg_id == -1)
    {
        fprintf (stderr, "msgget error %d, %s\n",
                 errno, strerror (errno));

        exit(1);
    }

    //2.输出创建成功的消息队列的key
    printf("msg key = %#x\n",key);

    //3.可以选择验证key的生成算法  
    //project_id(8bit) + device_number(8bit) + inode_number(16bit)
	//key = (proj_id & 0xFF) << 24 | (st_dev & 0xFF) << 16 | (st_ino & 0xFFFF);
    struct stat file_stat;
    stat(".",&file_stat);

    printf("%#lx\n",file_stat.st_dev); //输出文件的设备编号
    printf("%#lx\n",file_stat.st_ino); //输出文件的文件编号
    printf("0xFFFFFF01\n");

    return 0;
}