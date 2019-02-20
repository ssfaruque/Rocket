CC := gcc
CFLAGS := -Wall -Werror 

TARGET := librdsm.a


all: $(TARGET)



clean:
	$(RM) $(TARGET)
