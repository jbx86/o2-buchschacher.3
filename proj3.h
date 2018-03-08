#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/msg.h>

#define KEY 1077 
#define MSGSZ 256
#define NPS 1000000000
#define MAXCHILD 10
#define MAXDUR 1000000

typedef struct msgbuf{
	long mtype;
	char mtext[MSGSZ];
} message_buf;

typedef struct {
	int sec, nano;
} sim_time;


// Add a number of nanoseconds to a time
void simadd(sim_time *time, int inc) {
	time->nano += inc;
	if (time->nano >= NPS) {
		time->sec++;
		time->nano = time->nano % NPS;
	}
}

// Retrun number of nanoseconds that have passed since epoch
int simdiff(sim_time simclk, sim_time epoch) {
	if (simclk.sec < epoch.sec) {
		fprintf(stderr, "error calling simdiff function\n");
		exit(1);
	}

	while (simclk.sec > epoch.sec) {
		simclk.sec--;
		epoch.nano += NPS;
	}
	return (simclk.nano - epoch.nano);
}
