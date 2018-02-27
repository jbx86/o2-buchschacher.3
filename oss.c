#include <stdio.h>
#include <unistd.h>


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
		/*	case '?':
				if (optopt == 'c')
					printf("Option %c requires an argument\n", optopt
				break;
		*/
		}
	}
	printf("Arguments: h=%d, x=%d, filename=%s, z=%d\n", hflag, x, filename, z);

	// Open log file
	if ((fp = fopen(filename, "w")) == NULL) {
		fprintf(stderr, "Could not open log file\n");
		return 0;
	}

	// Initialize shared memory clock to zero
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
