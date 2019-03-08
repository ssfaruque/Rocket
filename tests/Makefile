CC := gcc
CFLAGS := -Wall -Werror -g
LIBS :=  -lrdsm
LIBPATHS := -L ../lib
INCLUDEPATHS := -I ../src
TARGETS := client server

.PHONY := buildlib all clean

all: buildlib $(TARGETS)

client: client.c 
	@echo "Building client program..."
	$(CC) $(CFLAGS) $^ $(LIBPATHS) $(LIBS) -pthread $(INCLUDEPATHS) -o $@

server: server.c 
	@echo "Building server program..."
	$(CC) $(CFLAGS) $^ $(LIBPATHS) $(LIBS) -pthread $(INCLUDEPATHS) -o $@

buildlib:
	make -C ..

clean:
	$(RM) $(TARGETS)