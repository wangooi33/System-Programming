#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
int main(int argc,char *argv[])
{
    int child_state;
    
    pid_t child_pid=fork();
    if(child_pid>0)//父进程
    {
        printf("my is parent,my pid is %d,my child pid is %d\n",getpid(),child_pid);
        wait(&child_state);
    }
    else if(child_pid==0)//子进程
    {
        printf("my is child,my pid is %d,my parent pid is %d\n",getpid(),getppid());

        //打算让子进程加载新的代码段和数据段，也就是让子进程执行新的可执行文件
        //execl("/usr/bin/ps","ps",NULL);   //execl的写法
        system("/usr/bin/ps");
    }
    else
    {
        printf("child process fork error\n")
        return -1;
    }

    return 0;//主函数调用return，系统会间接调用exit()。
}