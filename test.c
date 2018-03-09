#include<stdio.h>
#include<signal.h>
#include<unistd.h>

void sig_handler(int signo)
{
	break;
}

int main(void)
{
	signal(SIGALRM, sig_handler);
	signal(SIGINT, sig_handler);

	alarm(5);

  // A long long wait so that we can easily issue a signal to this process
	while(1) 
		sleep(1);
	printf("You're free!\n");
	return 0;
}
