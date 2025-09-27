CC = gcc
CFLAGS += -g -Wall -Werror -std=gnu11 -O2 -fsanitize=address
LDFLAGS += -lm

%: %.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f app *.o
	rm -f API test
