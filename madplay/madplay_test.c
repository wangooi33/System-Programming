
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
	int flag = 0;

	printf("/*******************************/\n");
	printf("\t\t 1:暂停\n");
	printf("\t\t 2:继续\n");
	printf("\t\t 3:结束\n");
	printf("\t\t 4:开始\n");
	printf("/*******************************/\n");


	while(1)
	{
		scanf("%d",&flag);
		printf("flag = %d\n",flag);
		switch(flag)
		{
			case 1: printf("已经暂停\n"); system("killall -STOP madplay");break;
			case 2: printf("已经继续\n"); system("killall -CONT madplay");break;
			case 3: printf("已经结束\n"); system("killall -KILL madplay");break;
			case 4: printf("已经开始\n"); system("madplay -Q xxx.mp3 &");break;
		}
	}
	
	return 0;
}