#include <stdio.h>
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
    }
    else
    {
        printf("child process fork error\n")
        return -1;
    }

    return 0;//主函数调用return，系统会间接调用exit()。
}
