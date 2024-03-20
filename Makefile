CC = gcc
CFLAGS = -x c -std=gnu99 -g -Wall -Wno-deprecated-declarations -rdynamic $(shell pkg-config --cflags gtk+-3.0) -I ./Interface

LDLIBS = $(shell pkg-config --libs gtk+-3.0) -lm

C_SOURCES = $(wildcard ./Src/*.c)
EXECUTABLE = ./App/run

all: create-app compile

create-app:
	mkdir -p ./App

compile: $(C_SOURCES)
	$(CC) $(CFLAGS) $(C_SOURCES) $(LDLIBS) -o $(EXECUTABLE)

clean:
	rm -rf $(EXECUTABLE)