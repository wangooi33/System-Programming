/*
 * 操作流程：在进行条件判断前，先加锁（防止其他任务并发访问），成功加锁后判断条件是否允许，
 * 若条件允许，则直接操作临界资源，然后释放锁。
 * 若条件不允许，则进入条件量的等待队列中睡眠，并同时释放锁。
 * 
 * 在条件量中睡眠的任务，可以被其他任务唤醒，唤醒时重新判定条件是否允许程序继续执行，当然也是必须先加锁。
 * 
 * 条件量一般要跟互斥锁（或二值信号量）配套使用，互斥锁提供锁住临界资源的功能，条件量提供阻塞睡眠和唤醒的功能。
 * 
*/

int flag = 0;

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;  //互斥量
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;   //条件量
/*
 * pthread_cond_t cond;
 * pthread_cond_init(&cond, NULL);
 *
*/

//假设进程运行之后，线程A-->线程B: 线程A上锁，条件不满足，
//则线程A挂起（队列中有1个线程），线程B上锁，条件不满足，则线程B挂起（队列中有2个线程）
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