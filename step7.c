#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

#define NUMBER_OF_PHILOS 12

typedef struct s_philo	t_philo;

time_t	ft_time(void)
{
	struct timeval		tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

typedef struct s_diner
{
	pthread_mutex_t	mutex;
	int				number;
	pthread_t		threads[NUMBER_OF_PHILOS];
	t_philo			**philosophers;
	time_t			start_time;
}	t_diner;

typedef struct s_philo
{
	int				id;
	t_diner			*diner;
}	t_philo;


void	*routine(void *content)
{
	t_philo	*philo;

	philo = (t_philo *)content;
	printf("ABOUT TO LOCK THE MUTEX : PHILO NUMBER %d\n", philo->id);
	pthread_mutex_lock(&philo->diner->mutex); //comment these two lines to see the difference
	printf("Locked by nb %d at time = %ldms.\n", philo->id, ft_time() - philo->diner->start_time);
	usleep(5000000);
	pthread_mutex_unlock(&philo->diner->mutex); //Mutexs will put on "pause" the treads waiting for it!
	printf("UNLOCKED by nb %d at time = %ldms.\n\n", philo->id, ft_time() - philo->diner->start_time);
	return (NULL);
}

//This shows you how extactly mutexes impact your program.
//Right before they want to lock, it will be printed.
//When they get to lock the mutex, a message will appear.
//with and without mutexes, it shows the huge difference in behaviour.

int	main()
{
	int				i;
	pthread_mutex_t	mutex;
	t_diner			diner;

	pthread_mutex_init(&mutex, NULL);
	i = 0;
	diner.mutex = mutex;
	diner.number = 0;
	diner.philosophers = malloc(sizeof(t_philo *) * NUMBER_OF_PHILOS);
	diner.start_time = ft_time();
	printf("Starting time : %ld.\n", ft_time() - diner.start_time);
	sleep(1);
	while (i < NUMBER_OF_PHILOS)
	{
		diner.philosophers[i] = malloc(sizeof(t_philo));
		diner.philosophers[i]->id = i;
		diner.philosophers[i]->diner = &diner;
		pthread_create(&diner.threads[i], NULL, &routine, diner.philosophers[i]);
		i++;
	}
	i = 0;
	while (i < NUMBER_OF_PHILOS)
	{
		pthread_join(diner.threads[i], NULL);
		i++;
	}
	pthread_mutex_destroy(&mutex);
}