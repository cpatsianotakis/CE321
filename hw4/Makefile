all:	
	pkg-config fuse --cflags --libs
	pkg-config libssl --cflags --libs
	gcc -g bbfs.c log.c -lcrypto -D_FILE_OFFSET_BITS=64 -lfuse -o bbfs -o bbfs
clean:
	rm -rf ./bbfs
