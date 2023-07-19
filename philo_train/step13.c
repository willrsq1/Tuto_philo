#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

#define NUMBER_OF_PHILOS 4
#define TIME_TO_EAT 200
#define TIME_TO_SLEEP 200
#define TIME_TO_THINK 10
#define TIME_BEFORE_DEATH 400
#define EATING 1
#define SLEEPING 2
#define THINKING 3
#define DEAD 4
#define LOCK 1
#define UNLOCK 2
#define OWN_FORK 1
#define OTHER_FORK 2
#define YES	1
#define NO 0
#define SOMEBODY_DIED 1
#define NO_ONE_HAS_DIED 7

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
	pthread_mutex_t	death_lock;
	int				kill_all_philos;
	pthread_t		threads[NUMBER_OF_PHILOS];
	t_philo			**philosophers;
	time_t			start_time;
}	t_diner;

typedef struct s_philo
{
	int				id;
	time_t			time_of_meal;
	pthread_mutex_t	*own_fork;
	pthread_mutex_t	*left_neighbour_fork;
	t_diner			*diner;
}	t_philo;

int	ft_death_function(t_philo *philo);
int	ft_is_anyone_dead(t_philo *philo);

void	ft_usleep(t_philo *philo, time_t time)
{
	time_t	starting_time;

	starting_time = ft_time();
	while (1)
	{
		if (ft_time() > starting_time + time)
			return ;
		if (ft_death_function(philo) == SOMEBODY_DIED)
			return ;
		usleep(1000);
	}
}

void	ft_writing(t_philo *philo, int message)
{
	pthread_mutex_lock(&philo->diner->writing_lock);
	if (message == EATING)
		printf("Philo % 3d is \x1b[32meating\x1b[0m: %ldms.\n", philo->id, ft_time() - philo->diner->start_time);
	else if (message == SLEEPING)
		printf("Philo % 3d is \x1b[96msleeping\x1b[0m: %ldms.\n", philo->id, ft_time() - philo->diner->start_time);
	else if (message == THINKING)
		printf("Philo % 3d is \x1b[31mthinking\x1b[0m: %ldms.\n", philo->id, ft_time() - philo->diner->start_time);
	else if (message == DEAD)
		printf("Philo % 3d is \x1b[31mDEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD\x1b[0m: %ldms.\n", philo->id, ft_time() - philo->diner->start_time);
	pthread_mutex_unlock(&philo->diner->writing_lock);
}

void	ft_forks(t_philo *philo, int action)
{
	if (action == LOCK)
	{
		pthread_mutex_lock(philo->own_fork);
		pthread_mutex_lock(philo->left_neighbour_fork);
	}
	if (action == UNLOCK)
	{
		pthread_mutex_unlock(philo->own_fork);
		pthread_mutex_unlock(philo->left_neighbour_fork);
	}
}


//The death function : How does it work ?
//When a philo hasn't eaten in a certain time, the first if will proc.
//The "ft_is_anyone_dead" will check for the "kill_all_philos" variable's value.
//If the if mentionned above has been proced by a thread, the value will be set to YES and we will now a philo has died.
//The death function is only used in ft_usleep for now. But it will now stop when a philo dies: not only when time >10sec.

int	ft_death_function(t_philo *philo)
{
	if (philo->time_of_meal + TIME_BEFORE_DEATH < ft_time())
	{
		pthread_mutex_lock(&philo->diner->death_lock);
		if (philo->diner->kill_all_philos == NO)
			ft_writing(philo, DEAD);
		philo->diner->kill_all_philos = YES;
		pthread_mutex_unlock(&philo->diner->death_lock);
		return (SOMEBODY_DIED);
	}
	return (ft_is_anyone_dead(philo));
}

int	ft_is_anyone_dead(t_philo *philo)
{
	pthread_mutex_lock(&philo->diner->death_lock);
	if (philo->diner->kill_all_philos == YES)
	{
		pthread_mutex_unlock(&philo->diner->death_lock);
		return (SOMEBODY_DIED);
	}
	pthread_mutex_unlock(&philo->diner->death_lock);
	return (NO_ONE_HAS_DIED);
}



void	*routine(void *content)
{
	t_philo	*philo;

	philo = (t_philo *)content;
	while (1)
	{
		ft_forks(philo, LOCK);//		THIS
		philo->time_of_meal = ft_time();
		ft_writing(philo, EATING);//	IS
		ft_usleep(philo, TIME_TO_EAT);//EATING
		ft_forks(philo, UNLOCK);//		CYCLE
	

		ft_writing(philo, SLEEPING);//		SLEEPING
		ft_usleep(philo, TIME_TO_SLEEP);//	CYCLE
		ft_writing(philo, THINKING); // nothing
		ft_usleep(philo, TIME_TO_THINK);//	CYCLE
		if (ft_time() - philo->diner->start_time > 10000)
			return (NULL); //we still kill it a 10sec
		if (ft_death_function(philo) == SOMEBODY_DIED)
			return (NULL);
	}
	return (NULL);
}

//NICE ! BUT WHAT HAPPENS WHEN ONE DIES ?
// Other messages still get printed. That's bad. To fix it, how can we do ? Where can we put a mutex_lock?
// yes ! in the printing function
// also, let's add the "meals needed" variable. In order to stop after a philo eats X times.

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
	pthread_mutex_init(&diner.death_lock, NULL);
	diner.kill_all_philos = NO;
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
	pthread_mutex_destroy(&diner.death_lock);
}