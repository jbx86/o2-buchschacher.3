#include "proj3.h"

int main(int argc, char *argv[]) {

	key_t key = KEY;	// IPC key
	int shmid;		// Shared memory ID
	sim_time *shmclk;	// Pointer to simulated clock in shared memory
	sim_time epoch;		// Time when this program starts to execute
	int runtime;		// Nanoseconds the process has run for
	int msqid;		// Message queue ID
	message_buf buf;	// Message buffer
	size_t buf_length;	// Length of send message buffer
	int duration;		// Duration of user program
	int increment = 10;	//

// IPC Setup:

	// Locate shared memory segment
        if ((shmid = shmget(key, sizeof(sim_time), 0666)) < 0) {
                perror("user: shmget");
                exit(1);
        }

        // Attach shared clock
        if ((shmclk = shmat(shmid, NULL, 0)) == (sim_time *) -1) {
                perror("user: shmat");
                exit(1);
        }

	// Locate message queue
	if ((msqid = msgget(key, 0666)) < 0) {
		perror("user: msgget");
		exit(1);
	}

	// Block until message recieved
	if (msgrcv(msqid, &buf, MSGSZ, 2, 0) < 0) {
		perror("user: msgrcv");
		exit(1);
	}

//----- Critical Section -------------------------

//printf("\t%ld: message recieved, entering critical section\n", getpid());

//printf("seting epoch...\n");

	// Read the system clock
	epoch = *shmclk;

//printf("epoch set. \n");

	// Generate random duration
	srand(time(NULL) ^ getpid());
	duration = (rand() % MAXDUR) + 1;
	runtime = 0;

//printf("duration(%d) and runtime(%d) initialized. \n", duration, runtime);

	while (runtime < duration) {
		//printf("%09d / %0d\n", runtime, duration);
		// Determine how much work is being done this round
		runtime = simdiff(*shmclk, epoch);
		if ((duration - runtime) < increment) {
			increment = duration - runtime;
		}

		// Increment to show work done this round
		simadd(shmclk, increment);
	}

	//printf("\t%ld: critical section finished, sending message\n", getpid());

        // Write termination message to buffer
	buf.mtype = 1;
	sprintf(buf.mtext, "Child pid %ld started at %d.%09d, is terminating at time %d.%09d because it ran for  0.%09d", (long)getpid(), epoch.sec, epoch.nano, shmclk->sec, shmclk->nano, runtime);
	buf_length = strlen(buf.mtext) + 1;

//------------------------------------------------

	// Send termination message to the queue
	if (msgsnd(msqid, &buf, buf_length, 0) < 0) {
		perror("user: msgsend");
        }

	exit(0);
}
