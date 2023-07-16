#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

#define NUMBER_OF_PHILOS 60
#define TIME_TO_EAT 1000
#define TIME_TO_SLEEP 1000

typedef struct s_philo	t_philo;

time_t	ft_time(void)
{
	struct timeval		tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

typedef struct s_diner
{
	pthread_mutex_t	forks[NUMBER_OF_PHILOS];
	pthread_mutex_t	writing_lock;
	int				number;
	pthread_t		threads[NUMBER_OF_PHILOS];
	t_philo			**philosophers;
	time_t			start_time;
}	t_diner;

typedef struct s_philo
{
	int				id;
	pthread_mutex_t	*own_fork;
	pthread_mutex_t	*left_neighbour_fork;
	t_diner			*diner;
}	t_philo;

void	ft_usleep(t_philo *philo, time_t time)
{
	time_t	starting_time;

	starting_time = ft_time();
	while (1)
	{
		if (ft_time() > starting_time + time)
			return ;
		if (ft_time() - philo->diner->start_time > 10000)
			return ;
		usleep(1000);
	}
}

//Makes them all die when time = 10sec.. Not when they starve to death bc of "ms_time_to_die".

void	*routine(void *content)
{
	t_philo	*philo;

	philo = (t_philo *)content;
	while (1)
	{
		pthread_mutex_lock(philo->own_fork);
		pthread_mutex_lock(philo->left_neighbour_fork);
		pthread_mutex_lock(&philo->diner->writing_lock); //FIX THE PRINTF
		printf("Philo %03d is \x1b[32meating\x1b[0m: %ldms.\n", philo->id, ft_time() - philo->diner->start_time);
		pthread_mutex_unlock(&philo->diner->writing_lock);//FIX THE PRINTF
		ft_usleep(philo, TIME_TO_EAT);
		pthread_mutex_unlock(philo->own_fork);
		pthread_mutex_unlock(philo->left_neighbour_fork);
		pthread_mutex_lock(&philo->diner->writing_lock);//FIX THE PRINTF
		printf("Philo %03d is \x1b[96msleeping\x1b[0m: %ldms.\n", philo->id, ft_time() - philo->diner->start_time);
		pthread_mutex_unlock(&philo->diner->writing_lock);//FIX THE PRINTF
		ft_usleep(philo, TIME_TO_SLEEP);
		pthread_mutex_lock(&philo->diner->writing_lock);//FIX THE PRINTF
		printf("Philo %03d is \x1b[31mthinking\x1b[0m: %ldms.\n", philo->id, ft_time() - philo->diner->start_time);
		pthread_mutex_unlock(&philo->diner->writing_lock);//FIX THE PRINTF
		if (ft_time() - philo->diner->start_time > 10000)
			return (NULL);
	}
	return (NULL);
}

int	main()
{
	int				i;
	t_diner			diner;

	i = 0;
	while (i < NUMBER_OF_PHILOS) //mutex creation, 1/philo
	{
		pthread_mutex_init(&diner.forks[i], NULL);
		i++;
	}
	i = 0;
	pthread_mutex_init(&diner.writing_lock, NULL);
	diner.number = 0;
	diner.philosophers = malloc(sizeof(t_philo *) * NUMBER_OF_PHILOS);
	diner.start_time = ft_time();
	printf("Starting time : %ld.\n", ft_time() - diner.start_time);
	while (i < NUMBER_OF_PHILOS)
	{
		diner.philosophers[i] = malloc(sizeof(t_philo));
		diner.philosophers[i]->id = i;
		diner.philosophers[i]->diner = &diner;
		diner.philosophers[i]->own_fork = &diner.forks[i]; //giving them two forks
		diner.philosophers[i]->left_neighbour_fork = &diner.forks[(i + 1) % NUMBER_OF_PHILOS]; // philo 0: forks 1 and 2. philo nb MAX: forks max and 0.
		pthread_create(&diner.threads[i], NULL, &routine, diner.philosophers[i]);
		i++;
	}
	i = 0;
	while (i < NUMBER_OF_PHILOS)
	{
		pthread_join(diner.threads[i], NULL); //join the threads BEFORE destroying the mutexes
		i++;
	}
	i = 0;
	while (i < NUMBER_OF_PHILOS)
	{
		pthread_mutex_destroy(&diner.forks[i]);
		i++;
	}
	pthread_mutex_destroy(&diner.writing_lock);
}