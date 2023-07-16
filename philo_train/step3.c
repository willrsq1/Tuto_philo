#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void	*routine(void *content)
{
	int	*number;
	int	i;

	number = (int *)content;
	i = 0;
	while (i < 1000000)
	{
		*number += 1;
		i++;
	}
	printf("Number is now = %d.\n", *number);
	return (NULL);
}

int	main()
{
	pthread_t	threads[10];
	int			number;
	int			i;

	i = 0;
	number = 0;
	printf("At initializaiton, number = %d.\n", number);
	while (i < 10)
	{
		pthread_create(&threads[i], NULL, &routine, &number);
		printf("Thread %d created.\n", i);
		// usleep(1000000);
		i++;
	}
	i = 0;
	while (i < 10)
	{
		pthread_join(threads[i], NULL);
		printf("Thread %d destroyed.\n", i);
		i++;
	}
}