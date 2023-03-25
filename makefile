
CC ?= gcc

PROG = dungeonGenerator

OBJ = cstr.o vec2.o arg.o rng.o cairo_renderer.o bsp.o map.o dungeon.o

CFLAGS = -I/usr/include/cairo
LDFLAGS = -lcairo -lm



all: $(PROG)


$(PROG): main.c $(OBJ)
	$(CC) -g -o $@ $^ $(CFLAGS) $(LDFLAGS)

.o: .c .h
	$(CC) -g -c $@ $^


clean:
	rm $(OBJ)

