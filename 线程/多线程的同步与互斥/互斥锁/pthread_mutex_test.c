
int x;

pthread_mutex_t  xxx;


void *task(void *arg)
{
	while(1)
	{
		pthread_mutex_lock(&xxx); 	//上锁

		x = 10;

		pthread_mutex_unlock(&xxx); //解锁

	}
}



int main(int argc, char const *argv[])
{
	

	while(1)
	{
	
		pthread_mutex_lock(&xxx); 	//上锁
		
		x = 20;

		pthread_mutex_unlock(&xxx); //解锁

	}


	return 0;
}