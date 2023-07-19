#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

typedef struct s_philo	t_philo;

typedef struct s_diner
{
	pthread_mutex_t	mutex;
	int				number;
	pthread_t		threads[10];
	t_philo			**philosophers;
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
	while (i < 3)
	{
		pthread_mutex_lock(&philo->diner->mutex); //comment these two lines to see the difference
		philo->diner->number += 1;
		printf("Hey from philosopher %d ! Number is now %d.\n", philo->id, philo->diner->number);
		usleep(500000);
		pthread_mutex_unlock(&philo->diner->mutex); // what happens if you comment only one of the lines ?
		i++;
	}
	return (NULL);
}

//Adding a struct per threads (= a philosopher).
//we can see which threads (= philosopher) changes the number.

//gcc -Wall -Wextra -Werror -fsanitize=thread ex5.c && ./a.out

int	main()
{
	int				i;
	pthread_mutex_t	mutex;
	t_diner			diner;

	pthread_mutex_init(&mutex, NULL);
	i = 0;
	diner.mutex = mutex;
	diner.number = 0;
	diner.philosophers = malloc(sizeof(t_philo *) * 10);
	// printf("At initializaiton, number = %d.\n", diner.number);
	while (i < 10)
	{
		diner.philosophers[i] = malloc(sizeof(t_philo));
		diner.philosophers[i]->id = i; //giving the philos their id
		diner.philosophers[i]->diner = &diner; //giving the address of the main struct to all philos
		pthread_create(&diner.threads[i], NULL, &routine, diner.philosophers[i]);
		// printf("Thread %d created.\n", i);
		i++;
	}
	i = 0;
	while (i < 10)
	{
		pthread_join(diner.threads[i], NULL);
		// printf("Thread %d destroyed.\n", i);
		i++;
	}
	printf("Number is now = %d.\n", diner.number);
	pthread_mutex_destroy(&mutex);
}