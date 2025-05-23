

int flag = 0;

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;  //互斥量
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;   //条件量


//假设进程运行之后，线程A-->线程B: 线程A上锁，条件不满足，则线程A挂起（队列中有1个线程），线程B上锁，条件不满足，则线程B挂起（队列中有2个线程）
int x = 100,y = 200;

//子线程A
taskA
{
	//设置分离属性，终止时自动释放资源
	pthread_detach(pthread_self());

	while(1)
	{
		//1.先对互斥量进行上锁
		pthread_mutex_lock(&mut);

		//2.判断条件(x>y),如果满足该条件，则对临界资源进行访问 设置flag = 1;
		while(x<=y)
		{
			pthread_cond_wait(&cond, &mut); //进入阻塞态，此时会自动解锁
		}


		flag = 1;

		//3.要对互斥量进行解锁
		pthread_mutex_unlock(&mut);

		//线程终止
		pthread_exit(pthread_self());
	}
	
}

//子线程B
taskB 
{
	//设置分离属性，终止时自动释放资源
	pthread_detach(pthread_self());

	while(1)
	{
		//1.先对互斥量进行上锁
		pthread_mutex_lock(&mut);

		//2.判断条件(x>y),如果满足该条件，则对临界资源进行访问 输出flag变量的值
		while(x<=y)
		{
			pthread_cond_wait(&cond, &mut); //进入阻塞态，此时会自动解锁
		}

		printf("flag = %d\n",flag);

		//3.要对互斥量进行解锁
		pthread_mutex_unlock(&mut);

		//线程终止
		pthread_exit(pthread_self());
	}
}



//子线程C
taskC
{
	//设置分离属性，终止时自动释放资源
	pthread_detach(pthread_self());

	while(1)
	{
		//1.先对互斥量进行上锁
		pthread_mutex_lock(&mut);

		//2.判断条件是否满足(x>y),如果满足，则唤醒所有使用相同条件量的处于挂起状态的线程(2个)
		if(x>y)
		{
			pthread_cond_broadcast(&cond);
		}
		
		//3.要对互斥量进行解锁
		pthread_mutex_unlock(&mut);

		//线程终止
		pthread_exit(pthread_self());
	}
}


//子线程D
taskD
{	
	//设置分离属性，终止时自动释放资源
	pthread_detach(pthread_self());

	while(1)
	{
		//1.先对互斥量进行上锁
		pthread_mutex_lock(&mut);

		//2.访问临界资源，设置x>y
		x = 300;
		y = 100;
		//3.要对互斥量进行解锁
		pthread_mutex_unlock(&mut);

		//线程终止
		pthread_exit(pthread_self());
	}
}