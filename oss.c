#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>

#define DEBUG
#define SHKEY 12345
#define PERM (S_IRUSR | S_IWUSR)

typedef struct {
	int sec;
	int nano;
} clock;

static clock *sim_clock;

int main(int argc, char *argv[]) {

	int opt;
	int hflag = 0;	// -h flag
	int x = 5;	// Maximum number of slave processes
	char *filename = NULL;	// Log file to be used
	int z = 20;	// Time in seconds when the master will terminat

	FILE *fp;

	int i;
	int pid;

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
	key_t key = 12345;

	// Make a key
/*	if ((key = ftok(".", 'x')) == -1) {
		fprintf(stderr, "Failed to derive key from filepath\n");
		return 1; 
	}
*/

	// Create memory segment
	if ((shmid = shmget(key, sizeof(clock), IPC_CREAT | 0666)) == -1) {
		fprintf(stderr, "Failed to create shared memory segment\n");
		return 1;
	}

	// Attach to memory segment
	sim_clock = (clock *) shmat(shmid, NULL, 0);
	sim_clock->sec = 1;
	sim_clock->nano = 2;

	printf("Parent clock: %d:%d at %d\n", sim_clock->sec, sim_clock->nano, sim_clock);

	// Fork off appropriate number of child processes
	for (i = 0; i < x; i++) {
		pid = fork();
		if (pid == 0) {
			//printf("Child %d at %d:%d\n", getpid(), sim_clock->sec, sim_clock->nano);
			execlp("./user", "user", sim_clock);
			return 1;
		}
		else {
			printf("Parent %d spawned process %d\n", getpid(), pid);
		}
	}

	shmdt(sim_clock);
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
