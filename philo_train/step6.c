#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

#define NUMBER_OF_PHILOS 500

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
	int	i;

	philo = (t_philo *)content;
	i = 0;
	while (i < 20)
	{
		// pthread_mutex_lock(&philo->diner->mutex); //comment these two lines to see the difference
		philo->diner->number += 1;
		printf("Hey from philosopher %d at time = %ld ms ! number is: % 6d \n", philo->id, ft_time() - philo->diner->start_time, philo->diner->number);
		// usleep(500);
		// pthread_mutex_unlock(&philo->diner->mutex); // times get messed up because of the computer execution when no mutex !
		i++;
	}
	return (NULL);
}

//adding the time thingy !
//to do it, we get the time at start (ft_time).
//lets say its 1639XXXX; substracting it to the return of ft_time() later in the program will show the time since the start of the prg (in ms, with this particular version of ft_time).
//Please try with different numbers of philosophers, and see with/without mutexes/usleep the differences. Play with it until it's understood.
//notice how the messages get printed in bad orders when the mutexes are commented ? Messages with higher times will be before messages with lower times:
/* 
Hey from philosopher 235 at time = 1446 ms ! number is:   9984
Hey from philosopher 411 at time = 1417 ms ! number is:   9573
Hey from philosopher 411 at time = 1446 ms ! number is:   9985

*/

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
	// printf("At initializaiton, number = %d.\n", diner.number);
	while (i < NUMBER_OF_PHILOS)
	{
		diner.philosophers[i] = malloc(sizeof(t_philo));
		diner.philosophers[i]->id = i;
		diner.philosophers[i]->diner = &diner;
		pthread_create(&diner.threads[i], NULL, &routine, diner.philosophers[i]);
		// printf("Thread %d created.\n", i);
		i++;
	}
	i = 0;
	while (i < NUMBER_OF_PHILOS)
	{
		pthread_join(diner.threads[i], NULL);
		// printf("Thread %d destroyed.\n", i);
		i++;
	}
	printf("Number is now = %d.\n", diner.number);
	pthread_mutex_destroy(&mutex);
}