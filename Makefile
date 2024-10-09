CC=clang
CFLAGS=-Wall -Wextra -Wpedantic -std=c99
LIBS=-lSDL2 -lSDL2_image -lGL -lm
OBJECTS=main.o model.o util.o

.PHONY: run
run: planegame
	./planegame

planegame: $(OBJECTS)
	$(CC) $(CFLAGS) $(LIBS) $(OBJECTS) -o planegame

.depends: $(OBJECTS:.o=.c)
	$(CC) -MM $(OBJECTS:.o=.c) > .depends
include .depends

.PHONY: clean
clean:
	rm -f *.o
	rm -f planegame
	rm -f .depends
