CC = gcc
CFLAG = -Wall -Wextra -g -o

all: 
	$(CC) sender.c $(CFLAG) 1.o
	$(CC) measure.c $(CFLAG) 2.o

git:
	git add -A
	git commit -m "$m"
	git push

test:
	./test.sh

clean:
	rm -f *.o