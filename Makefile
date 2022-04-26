CC=gcc
AR=ar

CFLAGS=-Wall -std=c11 -static-libgcc -g
MACROS=-DMAKEFILECOMPILING
INCLUDES=./include

all: mkdir_o main.o gentok.a
	$(CC) $(CFLAGS) $(MACROS) -I$(INCLUDES) ./objects/main.o ./lib/debug/gentok.a -o ./bin/debug/gentok

release: mkdir_o main.o install
	$(CC) $(CFLAGS) $(MACROS) -I$(INCLUDES) ./objects/main.o ./lib/release/gentok.a -s -o ./bin/release/gentok

install: mkdirs_r script.o helper.o gentok.o
	$(AR) -rcs ./lib/release/gentok.a ./objects/script.o ./objects/helper.o ./objects/gentok.o

gentok.a: mkdirs_d script.o helper.o gentok.o
	$(AR) -rc ./lib/debug/gentok.a ./objects/script.o ./objects/helper.o ./objects/gentok.o

script.o: ./src/script.c
	$(CC) $(CFLAGS) $(MACROS) -I$(INCLUDES) -c ./src/script.c -o ./objects/script.o

helper.o: ./src/helper.c
	$(CC) $(CFLAGS) $(MACROS) -I$(INCLUDES) -c ./src/helper.c -o ./objects/helper.o

gentok.o: ./src/gentok.c
	$(CC) $(CFLAGS) $(MACROS) -I$(INCLUDES) -c ./src/gentok.c -o ./objects/gentok.o

main.o: ./main.c
	$(CC) $(CFLAGS) $(MACROS) -I$(INCLUDES) -c ./main.c -o ./objects/main.o

mkdirs_d:
	[ -d ./bin/debug ] || mkdir -p ./bin/debug
	[ -d ./lib/debug ] || mkdir -p ./lib/debug

mkdirs_r:
	[ -d ./bin/release ] || mkdir -p ./bin/release
	[ -d ./lib/release ] || mkdir -p ./lib/release

mkdir_o:
	[ -d ./objects ] || mkdir ./objects

clean:
	rm -rf ./bin ./lib ./objects
