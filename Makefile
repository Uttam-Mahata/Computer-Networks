CC = gcc
CFLAGS = -Wall -Wextra

all: serverPacketFwd clientPacketGen

serverPacketFwd: ServerPacketFwd.c
	$(CC) $(CFLAGS) -o serverPacketFwd ServerPacketFwd.c

clientPacketGen: ClientPacketGen.c
	$(CC) $(CFLAGS) -o clientPacketGen ClientPacketGen.c

clean:
	rm -f serverPacketFwd clientPacketGen
