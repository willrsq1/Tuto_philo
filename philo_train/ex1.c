#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void	*routine(void *content)
{
	char	*s;

	s = (char *)content;
	printf("%s\n", s);
	return (NULL);
}

int	main()
{
	pthread_t thread;
	pthread_t thread_2;
	pthread_t thread_3;
	char	s[6] = "Hello";
	char	s2[6] = "BEBEY";

	pthread_create(&thread, NULL, &routine, (void *)s);
	sleep(2);
	pthread_create(&thread_2, NULL, &routine, (void *)s2);
	usleep(2000000);
	pthread_create(&thread_3, NULL, &routine, (void *)"Arbesaaaaa");
	pthread_join(thread, NULL);
	pthread_join(thread_2, NULL);
	pthread_join(thread_3, NULL);
}