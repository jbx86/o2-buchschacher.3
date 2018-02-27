all: oss

oss: oss.c
	gcc -o oss oss.c

producer: producer.c
	gcc -o producer producer.c

consumer: consumer.c
	gcc -o consumer consumer.c
