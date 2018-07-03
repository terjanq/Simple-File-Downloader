CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -O2 
LFLAGS = -lm

DEPS = utils.h sock.h engine.h
OBJ = main.o sock.o engine.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

transport: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LFLAGS)

clean:
	rm $(OBJ)

distclean:
	rm transport $(OBJ)