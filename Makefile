CC = gcc
CFLAGS = -Wall -Wextra -pthread -lm
TARGET = prod-cons
SRCS = prod-cons.c queue.c
OBJS = $(SRCS:.c=.o)
HEADERS = prod-cons.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(CFLAGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean