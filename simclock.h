#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/msg.h>

#define KEY 5678
#define MSGSZ 128

typedef struct msgbuf{
	long mtype;
	char mtext[MSGSZ];
} message_buf;

typedef struct {
	int sec, nano;
} simclock;
