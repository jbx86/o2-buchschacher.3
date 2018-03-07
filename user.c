#include "proj3.h"

int main(int argc, char *argv[]) {

	key_t key = KEY;	// IPC key
	int shmid;		// Shared memory ID
	simclock *myclock;	// Simclock address
	int msqid;		// Message queue ID
	message_buf buf;	// Send message buffer
	size_t buf_length;	// Length of send message buffer
	int sim_dur;		// Durration of user program

// IPC Setup:

	// Locate shared memory segment
        if ((shmid = shmget(key, sizeof(simclock), 0666)) < 0) {
                perror("user: shmget");
                exit(1);
        }

        // Attach shared clock
        if ((myclock = shmat(shmid, NULL, 0)) == (simclock *) -1) {
                perror("user: shmat");
                exit(1);
        }

	// Locate message queue
	if ((msqid = msgget(key, 0666)) < 0) {
		perror("user: msgget");
		exit(1);
	}


// Critical Section:

	printf("\t%d: waiting for message\n", getpid());
	// Wait for message before executing critical section
	if (msgrcv(msqid, &buf, MSGSZ, 2, 0) < 0) {
		perror("oss: msgrcv");
		exit(1);
	}
	printf("\t%d: message recieved\n", getpid());

	// Read OSS clock and add a random increment of time to it
	srand(time(NULL));
	sim_dur = (rand() % 1000000) + 1;
        addtime(myclock, sim_dur);

        // Write termination message to buffer
	buf.mtype = 1;
	sprintf(buf.mtext, "Child pid %d is terminating at time %d.%09d because it reached %d.%09d, which lived for %d.%09d",
			getpid(), myclock->sec, myclock->nano, myclock->sec, myclock->nano, myclock->sec, myclock->nano);
	buf_length = strlen(buf.mtext) + 1;

	// Send termination message to the queue
	if (msgsnd(msqid, &buf, buf_length, 0) < 0) {
		perror("user: msgsend");
        }

	printf("\t%d: term message sent\n", getpid());

	exit(0);
}
