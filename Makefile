CC = gcc
CFLAGS = -g -Wall -Werror -std=gnu11 -O2 -fsanitize=address
LDFLAGS = -lm
TARGET = app
SRC = API.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET) *.o
