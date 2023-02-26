
CC ?= gcc

PROG = dungeonGenerator

OBJ = rng.o cairo_renderer.o bsp.o

CFLAGS = -I/usr/include/cairo
LDFLAGS = -lcairo -lm



all: $(PROG)


$(PROG): main.c $(OBJ)
	$(CC) -g -o $@ $^ $(CFLAGS) $(LDFLAGS)

.o: .c .h
	$(CC) -g -c $@ $^


clean:
	rm $(OBJ)

