#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <time.h>


#define KEY 12345

typedef struct {
	int sec, nano;
} simclock;
