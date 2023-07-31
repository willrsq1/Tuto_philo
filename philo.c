#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#define EATING 1
#define SLEEPING 2
#define THINKING 3
#define DONE_EATING 4
#define LOCK 1
#define UNLOCK 2
#define OWN_FORK 1
#define OTHER_FORK 2
#define YES	1
#define NO 0
#define SOMEBODY_DIED 1
#define NO_ONE_HAS_DIED 7
#define DELAY 1000
#define ERROR 1

typedef struct s_philo	t_philo;

time_t	ft_time(void)
{
	struct timeval		tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

typedef struct s_diner
{
	pthread_mutex_t	forks[200];
	pthread_mutex_t	writing_lock;
	pthread_mutex_t	death_lock;
	int				kill_all_philos;
	int				time_to_eat;
	int				time_to_sleep;
	int				time_to_think;
	int				time_to_die;
	int				number_of_meals_needed;
	int				number_of_philos;
	pthread_t		threads[200];
	t_philo			*philosophers[200];
	time_t			start_time;
}	t_diner;

typedef struct s_philo
{
	int				id;
	time_t			time_of_meal;
	int				meals_needed;
	pthread_mutex_t	*own_fork;
	pthread_mutex_t	*left_neighbour_fork;
	t_diner			*diner;
}	t_philo;

int	ft_death_function(t_philo *philo);
void	ft_end_of_program(t_diner *diner);

void	ft_philo(int argc, char **argv);









void	ft_usleep(t_philo *philo, time_t time)
{
	time_t	starting_time;

	if (ft_death_function(philo) == SOMEBODY_DIED)
		return ;
	starting_time = ft_time();
	while (ft_time() < starting_time + time)
	{
		if (ft_death_function(philo) == SOMEBODY_DIED)
			return ;
		usleep(100);
		if (ft_death_function(philo) == SOMEBODY_DIED)
			return ;
	}
}

void	ft_writing(t_philo *philo, char *message)
{
	pthread_mutex_lock(&philo->diner->writing_lock);
	if (philo->diner->kill_all_philos == NO)
		printf("% 10ld %d %s.\n", ft_time() - philo->diner->start_time, philo->id, message);
	pthread_mutex_unlock(&philo->diner->writing_lock);
}

void	ft_forks(t_philo *philo, int action)
{
	if (action == LOCK)
	{
		if (ft_death_function(philo) == SOMEBODY_DIED)
			return ;
		pthread_mutex_lock(philo->own_fork);
		ft_writing(philo, "has taken a fork");
		pthread_mutex_lock(philo->left_neighbour_fork);
		ft_writing(philo, "has taken a fork");
	}
	if (action == UNLOCK)
	{
		pthread_mutex_unlock(philo->own_fork);
		pthread_mutex_unlock(philo->left_neighbour_fork);
	}
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

int	ft_death_function(t_philo *philo)
{
	if (philo->time_of_meal + philo->diner->time_to_die <= ft_time())
	{
		pthread_mutex_lock(&philo->diner->death_lock);
		pthread_mutex_lock(&philo->diner->writing_lock);
		if (philo->diner->kill_all_philos == NO)
			printf("% 10ld %d died\n", ft_time() - philo->diner->start_time, philo->id);
		philo->diner->kill_all_philos = YES;
		pthread_mutex_unlock(&philo->diner->writing_lock);
		pthread_mutex_unlock(&philo->diner->death_lock);
		return (SOMEBODY_DIED);
	}
	return (ft_is_anyone_dead(philo));
}

void	*routine(void *content)
{
	t_philo	*philo;

	philo = (t_philo *)content;
	philo->time_of_meal = philo->diner->start_time + DELAY;
	usleep((philo->diner->start_time - ft_time()) * 0.8);
	while (ft_time() < philo->diner->start_time)
		;
	if (philo->id % 2 == 1)
	{
		ft_writing(philo, "is thinking");
		ft_usleep(philo, philo->diner->time_to_eat / 2);
	}
	while (philo->meals_needed != philo->diner->number_of_meals_needed)
	{
		ft_forks(philo, LOCK);
		if (ft_death_function(philo) == SOMEBODY_DIED)
			return (ft_forks(philo, UNLOCK), NULL);
		philo->time_of_meal = ft_time();
		philo->meals_needed += 1;
		ft_writing(philo, "is eating");
		ft_usleep(philo, philo->diner->time_to_eat);
		ft_forks(philo, UNLOCK);
		ft_writing(philo, "is sleeping");
		ft_usleep(philo, philo->diner->time_to_sleep);
		ft_writing(philo, "is thinking");
		ft_usleep(philo, philo->diner->time_to_think);
		if (ft_death_function(philo) == SOMEBODY_DIED)
			return (NULL);
	}
	return (NULL);
}

/* Functionnal philo. Add error management before completing.*/

time_t ft_time_to_think(t_diner *diner)
{
	time_t	time;

	if (diner->number_of_philos % 2 == 0)
	{
		time = diner->time_to_eat - diner->time_to_sleep - 5;
		if (time < 0)
			return (0);
		return (time);
	}
	else
	{
		time = diner->time_to_eat * 2 - diner->time_to_sleep - 5;
		if (time < 0)
			time = 0;
		return (time);
	}
	return (0);
}
















int	main(int argc, char **argv)
{
	if (!(argc == 5 || argc == 6))
		return (write(2, "4 or 5 args needed.\n", 20), 1);
	ft_philo(argc, argv);
	return (0);
}

int	ft_atoi(char *s)
{
	long long	number;
	int	i;

	i = 0;
	number = 0;
	while (s[i] && s[i] >= '0' && s[i] <= '9' && i < 10)
	{
		number = number * 10 + (s[i] - 48);
		i++;
	}
	if ((i == 10 && number > 2147483647) || i == 0 || s[i] != '\0')
	{
		printf("%s : Bad input. Positive ints only ! 1 - 200 Philos.\n", s);
		return (-1);
	}
	return (number);
}

int	ft_input_verification(t_diner *diner, int argc)
{
	if (diner->number_of_philos > 200)
		return (ERROR);
	if (diner->number_of_philos < 1)
		return (ERROR);
	if (diner->time_to_die < 1 || diner->time_to_eat < 1 || diner->time_to_sleep < 1)
		return (ERROR);
	if (argc == 6 && diner->number_of_meals_needed == -1)
		return (ERROR);
	if (diner->number_of_philos == 1)
	{
		printf("% 10d 1 has taken a fork.\n", 0);
		usleep(diner->time_to_die * 1000);
		printf("% 10d 1 died.\n", diner->time_to_die);
		return (ERROR);
	}
	return (0);
}

int	ft_init_diner(t_diner *diner, int argc, char **argv)
{
	int		i;

	i = 0;
	diner->number_of_philos = ft_atoi(argv[1]);
	diner->time_to_die = ft_atoi(argv[2]);
	diner->time_to_eat = ft_atoi(argv[3]);
	diner->time_to_sleep = ft_atoi(argv[4]);
	diner->number_of_meals_needed = -1;
	if (argc == 6)
		diner->number_of_meals_needed = ft_atoi(argv[5]);
	if (ft_input_verification(diner, argc) == ERROR)
		return (ERROR);
	diner->time_to_think = ft_time_to_think(diner);
	while (i < diner->number_of_philos)
	{
		pthread_mutex_init(&diner->forks[i], NULL);
		i++;
	}
	i = 0;
	pthread_mutex_init(&diner->writing_lock, NULL);
	pthread_mutex_init(&diner->death_lock, NULL);
	diner->kill_all_philos = NO;
	return (0);
}

int	ft_init_philosophers(t_diner *diner)
{
	int	i;

	i = 0;
	while (i < diner->number_of_philos)
	{
		diner->philosophers[i] = malloc(sizeof(t_philo));
		if (diner->philosophers[i] == NULL)
		{
			while (i-- > 0)
				free(diner->philosophers[i]);
			return (ERROR);
		}
		memset(diner->philosophers[i], 0, sizeof(t_philo));
		diner->philosophers[i]->id = i + 1;
		diner->philosophers[i]->meals_needed = 0;
		diner->philosophers[i]->diner = diner;
		diner->philosophers[i]->own_fork = &diner->forks[i];
		diner->philosophers[i]->left_neighbour_fork = &diner->forks[(i + 1) % diner->number_of_philos];
		i++;
	}
	return (0);
}

void	ft_philo(int argc, char **argv)
{
	t_diner	diner;
	int		i;

	i = 0;
	memset(&diner, 0, sizeof(t_diner));
	if (ft_init_diner(&diner, argc, argv) == ERROR)
		return ;
	if (ft_init_philosophers(&diner) == ERROR)
		return ;
	diner.start_time = ft_time() + DELAY;
	i = 0;
	while (i < diner.number_of_philos)
	{
		pthread_create(&diner.threads[i], NULL, &routine, diner.philosophers[i]);
		i++;
	}
	ft_end_of_program(&diner);
}



void	ft_end_of_program(t_diner *diner)
{
	int	i;

	i = 0;
	while (i < diner->number_of_philos)
	{
		pthread_join(diner->threads[i], NULL);
		i++;
	}
	i = 0;
	while (i < diner->number_of_philos)
	{
		pthread_mutex_destroy(&diner->forks[i]);
		free(diner->philosophers[i]);
		i++;
	}
	pthread_mutex_destroy(&diner->writing_lock);
	pthread_mutex_destroy(&diner->death_lock);
}