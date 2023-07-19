#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

typedef struct s_diner
{
	pthread_mutex_t	mutex;
	int				number;
}	t_diner;


void	*routine(void *content)
{
	t_diner	*diner;
	int	i;

	diner = (t_diner *)content;
	i = 0;
	while (i < 10000000)
	{
		pthread_mutex_lock(&diner->mutex); //comment these two lines to see the difference
		diner->number += 1;
		pthread_mutex_unlock(&diner->mutex); // do it with "time ./a.out" to see the diff !
		i++;
	}
	//if you put the mutexes outside of the while, it will be wayyy faster. But they will execute one after the other, not simultaneously. That's not what we want!
	return (NULL);
}

//ùutexes increase time fo execution, thats a future issue that has to be dealts with properly.
//But the results will always be perfect, because the number will always be changed only by one thread at a time, even if they operate at the same time

int	main()
{
	pthread_t		threads[10];
	int				i;
	pthread_mutex_t	mutex;
	t_diner			diner;

	pthread_mutex_init(&mutex, NULL);
	i = 0;
	diner.mutex = mutex;
	diner.number = 0;
	printf("At initializaiton, number = %d.\n", diner.number);
	while (i < 10)
	{
		pthread_create(&threads[i], NULL, &routine, &diner);
		printf("Thread %d created.\n", i);
		i++;
	}
	i = 0;
	while (i < 10)
	{
		pthread_join(threads[i], NULL);
		printf("Thread %d destroyed.\n", i);
		i++;
	}
	printf("Number is now = %d.\n", diner.number);
	pthread_mutex_destroy(&mutex);
}