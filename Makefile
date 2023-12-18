all: aht10-poll.c
	gcc -o aht10-poll aht10-poll.c

clean:
	rm aht10-poll
