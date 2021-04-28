##
# Minsweeper
#
# @file
# @version 0.1
CFLAGS = -g -O -Wall -W -pedantic -std=c99 -Wimplicit-fallthrough=0
LDLIBS = -lraylib -lm

all:
	gcc $(CFLAGS) main.c -o main.o $(LDLIBS)

# end
