all: oss user

oss: oss.c simclock.h
	gcc -o oss oss.c

user: user.c simclock.h
	gcc -o user user.c
