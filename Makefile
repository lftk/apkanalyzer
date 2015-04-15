SHELL = cmd
CC = gcc
CFLAGS = -g -Wall

all: analyzer

clean:
	del *.exe *.o

analyzer: main.o analyzer.o kmp.o
	$(CC) $(CFLAGS) -o apkanalyzer.exe main.o analyzer.o kmp.o

main: main.c
analyzer.o: analyzer.c analyzer.h
kmp.o: kmp.c kmp.h
