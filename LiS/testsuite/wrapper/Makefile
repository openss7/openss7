OBJS := wrapper.o
CC := gcc
CFLAGS := -g -Wall

all: wrapper

wrapper: $(OBJS)
	$(CC) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	-rm -f wrapper *.o
