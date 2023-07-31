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

//No mutexes : weird numbers because : if two threads do +1 on the value X, both put it to X=1, instead of having X+2 as the final result
//lets add mutexes then
//note that with the sleep, no pb: its bc threads will add 1000000 to the values before any other thread adds to it

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