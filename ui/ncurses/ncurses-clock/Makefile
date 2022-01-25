# Makefile for ncurses-clock program

# --- Define compiler and compiler options ---
CC = gcc
CFLAGS = -std=c99 -g -Wall -O2
INCLUDES = -lncurses -lpthread


# --- List program modules and associated object files ---
MODULES_LIST = \
	main \
	blockString \
	clockWindow \
	dateTimeModel \
	scanAndConfig \

OBJECT_FILES = $(MODULES_LIST:%=%.o)


# --- Define the build rules ---
all: main

main : $(OBJECT_FILES)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJECT_FILES) -o main

%.o: src/%.c
	$(CC) $(CFLAGS) $< -o $@ -c 

clean:
	rm -f main $(OBJECT_FILES)
