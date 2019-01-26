CC = gcc
CFALGS = -W -Wall -g -O0
TARGET = client
CLIENT_OBJECTS = peer.o queue.o stdin.o client.o msg.o debug.o option.o main2.o

all : $(TARGET)

client: $(CLIENT_OBJECTS)
	$(CC) $(CFLAGS) -g -o $@ $^


.c.o: 
	$(CC) $(CFLAGS) -g -c $< -o $@

clean:
	rm -f *.o 
	rm -f $(TARGET)

