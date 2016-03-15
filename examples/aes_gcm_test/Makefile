rm=/bin/rm -f
CC=gcc
DEFS= -DCONFIG_NATIVE_WINDOWS
INCLUDES=-I. -I../crypto -I../../sys
LIBS=

DEFINES= $(INCLUDES) $(DEFS)
CFLAGS= -std=c99 $(DEFINES) -O2 -fomit-frame-pointer -funroll-loops

all: aes128gcm

#aes-cbc.c  aes-gcm.c  aes-internal-dec.c  aes-internal-enc.c  aes-internal.c  aes-unwrap.c  aes-wrap.c  aes.h  aes_i.h  aes_wrap.h

aes128gcm: main.o aes-internal.o aes-internal-dec.o aes-internal-enc.o aes-gcm.o aes-unwrap.o aes-wrap.o  os.o
	$(CC) $(CFLAGS) -o aes128gcm aes-internal-dec.o aes-internal-enc.o aes-gcm.o aes-unwrap.o aes-wrap.o aes-internal.o main.o os.o

aes-internal.o: ../crypto/aes-internal.c
	$(CC) $(CFLAGS) -c ../crypto/aes-internal.c $(LIBS)

aes-internal-dec.o: ../crypto/aes-internal-dec.c
	$(CC) $(CFLAGS) -c ../crypto/aes-internal-dec.c $(LIBS)

aes-internal-enc.o: ../crypto/aes-internal-enc.c
	$(CC) $(CFLAGS) -c ../crypto/aes-internal-enc.c $(LIBS)
	
aes-gcm.o: ../crypto/aes-gcm.c
	$(CC) $(CFLAGS) -c ../crypto/aes-gcm.c $(LIBS) 

aes-unwrap.o: ../crypto/aes-unwrap.c
	$(CC) $(CFLAGS) -c ../crypto/aes-unwrap.c $(LIBS)
	
aes-wrap.o: ../crypto/aes-wrap.c
	$(CC) $(CFLAGS) -c ../crypto/aes-wrap.c $(LIBS) 
	
main.o: main.c
	$(CC) $(CFLAGS) -c main.c $(LIBS) 
	
os.o: ../../sys/os.c
	$(CC) $(CFLAGS) -c ../../sys/os.c $(LIBS) 
	
clean:
	$(rm) *.exe *.o core *~

