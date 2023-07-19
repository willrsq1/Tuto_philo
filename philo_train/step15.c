#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

// #define NUMBER_OF_PHILOS 55
// #define TIME_TO_EAT 200
// #define TIME_TO_SLEEP 100
// #define TIME_TO_THINK 10
// #define TIME_BEFORE_DEATH 230
// #define NUMBER_OF_MEALS_NEEDED 5
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

typedef struct s_philo	t_philo;

time_t	ft_time(void)
{
	struct timeval		tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

typedef struct s_diner
{
	pthread_mutex_t	*forks;
	pthread_mutex_t	writing_lock;
	pthread_mutex_t	death_lock;
	int				kill_all_philos;
	int				time_to_eat;
	int				time_to_sleep;
	int				time_to_think;
	int				time_to_die;
	int				number_of_meals_needed;
	int				number_of_philos;
	pthread_t		*threads;
	t_philo			**philosophers;
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
		usleep(100);
	}
}

void	ft_writing(t_philo *philo, int message)
{
	pthread_mutex_lock(&philo->diner->writing_lock);
	if (philo->diner->kill_all_philos == NO) //checking the death lock will change everything ! no message will be written after someone died.
	{
		if (message == EATING)
			printf("Philo % 3d is \x1b[32meating\x1b[0m: %ldms.\n", philo->id, ft_time() - philo->diner->start_time);
		else if (message == SLEEPING)
			printf("Philo % 3d is \x1b[96msleeping\x1b[0m: %ldms.\n", philo->id, ft_time() - philo->diner->start_time);
		else if (message == THINKING)
			printf("Philo % 3d is \x1b[31mthinking\x1b[0m: %ldms.\n", philo->id, ft_time() - philo->diner->start_time);
		else if (message == DONE_EATING)
			printf("Philo % 3d is \x1b[31mis done eating\x1b[0m: %ldms.\n", philo->id, ft_time() - philo->diner->start_time);
	}
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
			printf("Philo % 3d is \x1b[31mDEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD\x1b[0m: %ldms.\n", philo->id, ft_time() - philo->diner->start_time);
			//we don't use ft_writing, because ft_writing uses the same locks. It would block (you can try to see !)
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
	philo->time_of_meal = philo->diner->start_time + 1000;
	while (ft_time() < philo->diner->start_time) //makes them wait to start at the same time
		usleep(1000);
	if (philo->id % 2 == 1)
	{// THIS IS NECESSARY TO AVOID PHILOS BEEING STUCK WAITING FOR A LOCK TO UNLOCK, AND AVOIDING DEATH THAT WAY
		ft_writing(philo, THINKING);
		ft_usleep(philo, philo->diner->time_to_eat / 2);
	}
	while (1)
	{
		ft_forks(philo, LOCK);//		THIS
		if (ft_death_function(philo) == SOMEBODY_DIED)
			return (ft_forks(philo, UNLOCK), NULL);				//CHECK FOR DEATH AFTER UNLOCKING THE FORKS
		philo->time_of_meal = ft_time();
		philo->meals_needed += 1;
		ft_writing(philo, EATING);//	IS
		ft_usleep(philo, philo->diner->time_to_eat);//EATING
		ft_forks(philo, UNLOCK);//		CYCLE
	

		ft_writing(philo, SLEEPING);//		SLEEPING
		ft_usleep(philo, philo->diner->time_to_sleep);//	CYCLE
		if (philo->meals_needed == philo->diner->number_of_meals_needed)
			break ;
		ft_writing(philo, THINKING);
		if (philo->diner->number_of_philos % 2 == 1) //EXTREMELY IMPORTANT FOR WHEN THERE ARE AN ODD NUMBER OF PHILOS
			ft_usleep(philo, philo->diner->time_to_eat);//	CYCLE
		if (ft_death_function(philo) == SOMEBODY_DIED)
			return (NULL);
	}
	ft_writing(philo, DONE_EATING);
	return (NULL);
}

/*	1) Put the defines inside the main struct
	2) Replace all defines by the struct's var

	gotta do error handling
	*/

int	main(int argc, char **argv)
{
	int				i;
	t_diner			diner;

	i = 0;

	if (!(argc == 5 || argc == 6)) //check for the inputs
		return (write(2, "Bad args\n", 9), 1);
	diner.number_of_philos = atoi(argv[1]);
	diner.time_to_die = atoi(argv[2]); //get all the inputs
	diner.time_to_eat = atoi(argv[3]);
	diner.time_to_sleep = atoi(argv[4]);
	diner.number_of_meals_needed = -1;
	if (argc == 6)
		diner.number_of_meals_needed = atoi(argv[5]);
	//malloc the data with the right size
	diner.threads = malloc(sizeof(pthread_t) * diner.number_of_philos);
	diner.forks = malloc(sizeof(pthread_mutex_t) * diner.number_of_philos);

	//good !
	while (i < diner.number_of_philos) //mutex creation, 1/philo
	{
		pthread_mutex_init(&diner.forks[i], NULL);
		i++;
	}
	i = 0;
	pthread_mutex_init(&diner.writing_lock, NULL);
	pthread_mutex_init(&diner.death_lock, NULL);
	diner.kill_all_philos = NO;
	diner.philosophers = malloc(sizeof(t_philo *) * diner.number_of_philos);
	diner.start_time = ft_time() + 1000;
	printf("Starting time : %ld.\n", ft_time() - diner.start_time + 1000);
	while (i < diner.number_of_philos)
	{
		diner.philosophers[i] = malloc(sizeof(t_philo));
		diner.philosophers[i]->id = i;
		diner.philosophers[i]->meals_needed = 0;
		diner.philosophers[i]->diner = &diner;
		diner.philosophers[i]->own_fork = &diner.forks[i]; //giving them two forks
		diner.philosophers[i]->left_neighbour_fork = &diner.forks[(i + 1) % diner.number_of_philos]; // philo 0: forks 1 and 2. philo nb MAX: forks max and 0.
		pthread_create(&diner.threads[i], NULL, &routine, diner.philosophers[i]);
		i++;
	}
	i = 0;
	while (i < diner.number_of_philos)
	{
		pthread_join(diner.threads[i], NULL); //join the threads BEFORE destroying the mutexes
		i++;
	}
	free(diner.threads);
	i = 0;
	while (i < diner.number_of_philos)
	{
		pthread_mutex_destroy(&diner.forks[i]);
		free(diner.philosophers[i]);
		i++;
	}
	free(diner.philosophers);
	free(diner.forks);
	pthread_mutex_destroy(&diner.writing_lock);
	pthread_mutex_destroy(&diner.death_lock);
}