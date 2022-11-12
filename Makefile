CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0`
LIBS = `pkg-config --libs gtk+-3.0`
DIR = /usr/bin

compile:
	$(CC) $(CFLAGS) -o lemonscreen main.c $(LIBS)
install:
	cp -r main $(DIR)/lemonscreen
	chmod 755 $(DIR)/lemonscreen
remove:
	rm -f $(DIR)/lemonscreen
e.PHONY: compile
