CC=gcc
CFLAGS=-Wall

all: vm
vm: vm.c
	$(CC) $(CFLAGS) -o vm vm.c