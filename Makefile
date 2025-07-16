CC = gcc
CFLOGS = -Wall -g

TARGET = server
SRCS = server.c file_operations.c requests.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(CFLOGS) -o $@ $(OBJS)

	%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET) $(ARGS)

clean:
	rm -f $(TARGET) *.o