LIBS=-lpthread -lcrypt
CFLAGS=-O2 -g -Wall -Wextra
PROGS=cryptbench

all: $(PROGS)

cryptbench: cryptbench.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(PROGS)
	rm -f *.o