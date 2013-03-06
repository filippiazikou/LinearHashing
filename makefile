SOURCE1 = main1.c
SOURCE2 = main2.c
SOURCE3 = main3.c
SOURCE4 = LH.c

OBJECTS1 = main1.o LH.o BF.o
OBJECTS2 = main2.o LH.o BF.o
OBJECTS3 = main3.o LH.o BF.o

OUT1 = exe1
OUT2 = exe2
OUT3 = exe3
CC = gcc
FLAGS = -g -c

all:    $(OUT1) $(OUT2) $(OUT3)


$(OUT1): $(OBJECTS1)
	$(CC) -g $(OBJECTS1) -o $(OUT1) -lm
$(OUT2): $(OBJECTS2)
	$(CC) -g $(OBJECTS2) -o $(OUT2) -lm
$(OUT3): $(OBJECTS3)
	$(CC) -g $(OBJECTS3) -o $(OUT3) -lm


main1.o: main1.c
	$(CC) $(FLAGS) main1.c

main2.o: main2.c
	$(CC) $(FLAGS) main2.c

main3.o: main3.c
	$(CC) $(FLAGS) main3.c

LH.o: LH.c
	$(CC) $(FLAGS) LH.c

clean:
	rm -f exe1 exe2 exe3 main1.o main2.o main3.o LH.o hash*
