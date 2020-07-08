CC=gcc
CFLAGS=-I
# DEPS = functions.h red_black_tree.h bloom_filter.h prime.h listlist.h
# OBJ = main.o functions.o red_black_tree.o listlist.o prime.o bloom_filter.o

# %.o: %.c $(DEPS)
# 	$(CC) -c -o $@ $< $(CFLAGS)

# make: $(OBJ)
# 	$(CC) -o $@ $< $(CFLAGS)
all: runelection

runelection: main.o functions.o red_black_tree.o bloom_filter.o listlist.o prime.o
	$(CC) main.o functions.o red_black_tree.o bloom_filter.o listlist.o prime.o -o runelection

main.o: main.c
	$(CC) $(CFLAGS) main.c

functions.o: functions.c
	$(CC) $(CFLAGS) functions.c

red_black_tree.o: red_black_tree.c 
	$(CC) $(CFLAGS) red_black_tree.c 

bloom_filter.o: bloom_filter.c
	$(CC) $(CFLAGS) bloom_filter.c

listlist.o: listlist.c
	$(CC) $(CFLAGS) listlist.c

prime.o: prime.c
	$(CC) $(CFLAGS) prime.c
	
clean:
    rm *o runelection