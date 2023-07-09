##
# P2
#

CC = gcc
RM = rm -f
DFLAG = -g
GFLAGS = -Wall -Wextra
CFLAGS = $(DFLAG) $(GFLAGS) -c
LFLAGS = $(DFLAG) $(GFLAGS)

all: client server

client: UdpSocket.o ReliableConnection.o ReliableClient.o byteorder64.o
	$(CC) $(LFLAGS) byteorder64.o ReliableConnection.o UdpSocket.o ReliableClient.o -o client

server: UdpSocket.o ReliableConnection.o ReliableServer.o byteorder64.o
	$(CC) $(LFLAGS) byteorder64.o ReliableConnection.o UdpSocket.o ReliableServer.o -o server

UdpSocket.o: UdpSocket.h
	$(CC) $(CFLAGS) UdpSocket.c

ReliableConnection.o: UdpSocket.o ReliableConnection.h
	$(CC) $(CFLAGS) ReliableConnection.c

ReliableClient.o:
	$(CC) $(CFLAGS) ReliableClient.c

ReliableServer.o:
	$(CC) $(CFLAGS) ReliableServer.c

byteorder64.o: byteorder64.h
	$(CC) $(CFLAGS) byteorder64.c

clean:
	rm server client *.o


# end
