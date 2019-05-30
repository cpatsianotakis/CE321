OBJ = testbbfs bbfs

all: $(OBJ)

bbfs: bbfs.c log.c our_lib.c
	pkg-config fuse --cflags --libs
	pkg-config libssl --cflags --libs
	gcc -g bbfs.c log.c our_lib.c -lcrypto -D_FILE_OFFSET_BITS=64 -lfuse -o $@

testbbfs:
	gcc -Wall -g testbbfs.c -o $@

clean:
	rm -rf ./bbfs
	rm  testbbfs
