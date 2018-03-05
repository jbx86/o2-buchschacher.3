#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include "simclock.h"

int main(int argc, char *argv[]) {

	//clock *sim_clock = atoi(argv[1]);	
	struct simclock * sim_clock;
	int shmid;


// Create memory segment
	if ((shmid = shmget(KEY, sizeof(struct simclock), IPC_CREAT | 0666)) == -1) {
		fprintf(stderr, "Failed to create shared memory segment\n");
		return 1;
	}
// Attach to memory segment
	sim_clock = (struct simclock *) shmat(shmid, NULL, 0);

	printf("Child %d recieved value %p\n", getpid(), sim_clock);




	// Read OSS clock and add an increment of time to it.
	//Start by reading our simulated system clock.
	//Generate a random duration number from 1 to some maximum number of nanosecond constant (A reasonable value might be something like 1000000).
	//Loop continually over a critical section of code.
	//This critical section should be enforced through the use of message passing with msgsnd and msgrcv.
	//Critical section of code done use message passing, read the oss clock and then add an increment of nanoseconds to the clock.
	//If increment puts it past its duration, terminate.
	//Send a message to oss that it is going to terminate then terminate.
	//Send message to oss consisting of the current simulated system clock time that it decided to terminate on and total amount of seconds.nanoseconds that this child incremented the clock.
	//If the random interval of work exceeds the maximum allotted lifetime, do not work those extra nanoseconds and only increment the clock enough to hit that maximum time.
	//This checking of duration vs oss clock and messaging to master should only occur in the critical section.
	//If a user process gets inside the critical section and sees that its duration has not passed, cede the critical section to someone else and attempt to get back in the critical section.




	return 0;
}
