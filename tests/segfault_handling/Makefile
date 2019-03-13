CC := gcc
CFLAGS := -Wall -Werror
LIBS := -lrdsm
LIBPATHS := -L ../../lib
INCLUDEPATHS := -I ../../src
TARGETS := client server

.PHONY := buildlib all clean

all: $(TARGETS)

client: client.c 
	@echo "Building client program..."
	$(CC) $(CFLAGS) $^ $(LIBPATHS) $(LIBS) $(INCLUDEPATHS) -pthread -o $@

server: server.c 
	@echo "Building server program..."
	$(CC) $(CFLAGS) $^ $(LIBPATHS) $(LIBS) $(INCLUDEPATHS) -pthread -o $@

buildlib:
	make -C ..

clean:
	$(RM) $(TARGETS)
