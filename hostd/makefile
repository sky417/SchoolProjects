# Po Liu - 13623979
# Hao Zhang - 43324094
# James Zhan - 15314073
# Andrew Yoon - 10653038
#
# makefile for EECE315 PA4 Q7 hostd and child process
# 
#

CC = gcc
WARNING = -Wall
SOURCES = main.c process.c resource.c
CFLAGS = -std=gnu99
TARGET = hostd
CHILDSOURCES = child.c
CHILDTARGET = child

all: hostd child
	@echo "BOOM!"

hostd: $(SOURCES) 
	@echo "Something amazing is happening right now..."
	$(CC) $(CFLAGS) $(WARNING) $(SOURCES) -o $(TARGET)


child: $(CHILDSOURCES)
	@echo "Now compiling the child!"
	$(CC) $(CFLAGS) $(WARNING) $(CHILDSOURCES) -o $(CHILDTARGET)

clean: 
	@echo "Cleaning up the mess…"
	rm $(TARGET)
	rm $(CHILDTARGET)
