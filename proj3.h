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
#define MSGSZ 256
#define NPS 1000000000

typedef struct msgbuf{
	long mtype;
	char mtext[MSGSZ];
} message_buf;

typedef struct {
	int sec, nano;
} simclock;

void addtime(simclock *sc, int inc) {
	sc->nano += inc;
	if (sc->nano >= NPS) {
		sc->sec++;
		sc->nano = sc->nano % NPS;
	}
}
