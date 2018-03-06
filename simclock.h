#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/msg.h>

#define KEY 5678
#define MSGSZ 128

struct mymsg{
	long mtype;
	int mtext[MSGSZ];
} mymsg_t;

typedef struct {
	int sec, nano;
} simclock;
