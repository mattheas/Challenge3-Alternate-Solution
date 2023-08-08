PROGS = cooling_controller
CC = gcc
CFLAGS = -std=gnu99 -lm -lpthread -o

all: $(PROGS)

cooling_controller: cooling_controller.c
	$(CC) $(CFLAGS) cooling_controller cooling_controller.c
