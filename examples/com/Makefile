CC := gcc
CFLAGS := -Wall -Werror

TARGETS := client server

all: $(TARGETS)

client: client.c ../../src/com.c
	$(CC) $(CFLAGS) $^ -o $@

server: server.c ../../src/com.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) $(TARGETS)