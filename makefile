all: master producer consumer

master: master.c
	gcc -g -o master master.c makeargv.c

producer: producer.c
	gcc -o producer producer.c

consumer: consumer.c
	gcc -o consumer consumer.c
