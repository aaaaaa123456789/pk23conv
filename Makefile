CC ?= gcc

all:
	$(CC) -O3 -flto *.c libsrng/libsrng.c -o pk23conv

clean:
	rm -rf pk23conv pk23conv.exe

debug:
	$(CC) -ggdb *.c libsrng/libsrng.c -o pk23conv
