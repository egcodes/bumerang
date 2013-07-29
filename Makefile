CC = gcc
CFLAGS = -g -Wall
SOURCE = main.c get.c send.c connect.c bumerang_messages.c

TARGETS = bumerang

all: clean
	$(CC) $(CFLAGS) $(SOURCE) -o $(TARGETS)

clean:
	rm -f $(TARGETS)

install:
	cp $(TARGETS) /bin
