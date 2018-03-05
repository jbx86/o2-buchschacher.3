all: oss user simclock.h

oss: oss.c
	gcc -o oss oss.c

user: user.c
	gcc -o user user.c
