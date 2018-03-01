#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

#define PERM (S_IRUSR | S_IWUSR)

int main(int argc, char *argv[]) {

	int opt;
	int hflag = 0;	// -h flag
	int x = 5;	// Maximum number of slave processes
	char *filename = NULL;	// Log file to be used
	int z = 20;	// Time in seconds when the master will terminat

	FILE *fp;

	//Parse command line options
	while ((opt = getopt(argc, argv, "hs:l:t:")) != -1) {
		switch (opt) {
			case 'h':
				// Set -h flag
				hflag = 1;
				break;
			case 's':
				// Set max number of slave processes
				x = atoi(optarg);
				if (x < 1) {
					fprintf(stderr, "Invalid value entered, x must be an integer greater than zero\n");
					return 1;
				}
				break;
			case 'l':
				// Set log file to be used
				filename = optarg;
				break;
			case 't':
				// Set time when master will terminate
				z = atoi(optarg);
				if (z < 1) {
					fprintf(stderr, "Invalid value entered, z must be an integer than zero\n");
					return 1;
				}
				break;
		}
	}

	//printf("Arguments: h=%d, x=%d, filename=%s, z=%d\n", hflag, x, filename, z); // debug message

	// Open log file
	if ((fp = fopen(filename, "w")) == NULL) {
		fprintf(stderr, "Could not open log file\n");
		return 1;
	}

	// Initialize shared memory clock to zero
	int shmid;
	int *clock_s;
	int *clock_ns;
	key_t key;

	// Make a key
	if ((key = ftok(".", 'x')) == -1) {
		fprintf(stderr, "Failed to derive key from filepath\n");
		return 1; 
	}
	// Create memory segment
	if ((shmid = shmget(key, sizeof(int), IPC_CREAT | 0666)) == -1) {
		fprintf(stderr, "Failed to create shared memory segment\n");
		return 1;
	}
	// Attach to memory segment
	clock_s = (int *) shmat(shmid, NULL, 0);
	*clock_s = 0;
	printf("clock s:%d\n", *clock_s);

	int pid;
	pid = fork();
	if (pid == 0) {
		printf("clock s:%d\n", *clock_s);
		*clock_s = 6;
		printf("clock s:%d\n", *clock_s);

	}
	else {
		wait();
		printf("clock s:%d\n", *clock_s);
	}	

	clock_s = shmat(shmid, NULL, 0);

	shmdt(clock_s);
	shmctl(shmid, IPC_RMID, NULL);

	//fprintf(stderr, "shared memory id:%d\n", shmid);
	// Fork off the appropriate number of child processes.
	// wait for child processes to send a message.
	// When sent a message, output the contents of that message to a file
	// Go into the critical section to add 100 to the clock
	// Fork off another child.
	// After 2 seconds have passed in simulated system, terminate all children and master.
	//
	fclose(fp);
	return 0;
}
