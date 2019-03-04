CC := gcc
CFLAGS := -Wall -Werror

INCLUDEPATHS := -I ../../src
TARGET := list_test

.PHONY := all clean

all: $(TARGET)

$(TARGET): list_test.c ../../src/list.c
	$(CC) $(CFLAGS) $^ $(INCLUDEPATHS) -o $@

clean:
	$(RM) $(TARGET)