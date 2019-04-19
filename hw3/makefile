# The C compiler
CC = gcc

# Put any flags for the C compiler here
CFLAGS = -Wall -g

OBJECTS = test_hw3 memory_stats.o

all: $(OBJECTS) experiments

memory_stats.o:
	$(CC) $(CFLAGS) memory_stats.c -o memory_stats.o
	ar rcs libmemory_stats  memory_stats.o

experiments: memory_stats.o plateau rise
	$(CC) $(CFLAGS) experiment/experiments.c -o experiments libmemory_stats
	$(CC) $(CFLAGS) experiment/plateau.c -o experiment/plateau 
	$(CC) $(CFLAGS) experiment/rise.c -o experiment/rise 

test_hw3: memory_stats.o
	$(CC) $(CFLAGS) test_hw3.c -o test_hw3 libmemory_stats


.PHONY: clean

clean:
	rm $(OBJECTS) experiment/plateau experiment/rise experiments


