OBJS	= main_coordinator.o coordinator.o main_coach.o main_sorter.o main_heapsort.o main_quicksort.o quicksort.o
OUT	= coordinator coach sorter

OBJS0	= main_coordinator.o coordinator.o
SOURCE0	= main_coordinator.c coordinator.c
HEADER0	= coordinator.h
OUT0	= mysort

OBJS1	= main_coach.o
SOURCE1	= main_coach.c
HEADER1	= record.h
OUT1	= coach

OBJS2	= main_sorter.o main_heapsort.o main_quicksort.o quicksort.o
SOURCE2	= main_sorter.c main_heapsort.c main_quicksort.c quicksort.c
HEADER2	= record.h heapsort.h quicksort.h
OUT2	= sorter

CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = 

all: coordinator coach sorter

coordinator: $(OBJS0) $(LFLAGS)
	$(CC) -g $(OBJS0) -o $(OUT0)

coach: $(OBJS1) $(LFLAGS)
	$(CC) -g $(OBJS1) -o $(OUT1)

sorter: $(OBJS2) $(LFLAGS)
	$(CC) -g $(OBJS2) -o $(OUT2)

main_coordinator.o: main_coordinator.c
	$(CC) $(FLAGS) main_coordinator.c 

coordinator.o: coordinator.c
	$(CC) $(FLAGS) coordinator.c 

main_coach.o: main_coach.c
	$(CC) $(FLAGS) main_coach.c 

main_sorter.o: main_sorter.c
	$(CC) $(FLAGS) main_sorter.c 

main_heapsort.o: main_heapsort.c
	$(CC) $(FLAGS) main_heapsort.c 

main_quicksort.o: main_quicksort.c
	$(CC) $(FLAGS) main_quicksort.c 

quicksort.o: quicksort.c
	$(CC) $(FLAGS) quicksort.c 


clean:
	rm -f $(OBJS) $(OUT)
