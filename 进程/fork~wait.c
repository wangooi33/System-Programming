#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

//PID：进程ID
//PPID：父进程ID
//PGID：进程所在组的ID
//PCB：进程控制块


/*
 *进程控制块（Processing Control Block，缩写为PCB）
 *进程标识符（process identifier）PID
 *systemd进程，该进程是Linux系统运行的第一个进程，英文全称是system daemon，中文翻译为系统守护进程，系统中其他进程都是该进程的子进程。
 *进程间通信（IPC）
 *
 *
 *
*/

/***************************************************************/

/*
 * pid_t waitpid(pid_t pid, int *wstatus, int options);
 * 
 * pid:
 * 		-1:等待任意一个子进程
 * 		0 :等待本进程所在的进程组中的任意一个子进程
 * 	   <-1:等待组ID等于pid绝对值的进程组中的任意一个子进程
 * 	    >0:等待指定pid的子进程
 * 
 * option:
 * 			0:阻塞等待子进程的退出
 * 	  WNOHANG:若没有僵尸子进程，则函数立即返回
 *  WUNTRACED:当子进程暂停时函数返回
 * WCONTINUED:当子进程收到信号SIGCONT继续运行时函数返回
*/

/*
 *可以看到fork函数的返回值对于父进程和子进程而言是不同的,
 *fork函数在父进程中返回的 是创建成功的子进程的PID, 
 *fork函数在子进程中的返回值是0,当然,如果子进程失败则返回-1。
*/

int main(int argc,char *argv[])
{
    int child_state;
    
    pid_t child_pid=fork();
    if(child_pid>0)//父进程
    {
        printf("my is parent,my pid is %d,my child pid is %d\n",getpid(),child_pid);
        wait(&child_state);//阻塞父进程，直到子进程退出，将子进程终止状态存到child_state中。如果填NULL则不存储。
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
