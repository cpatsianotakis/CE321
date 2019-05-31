OBJ = finalTest makedirs

all: $(OBJ)

makedirs:
	mkdir -p mountdir
	mkdir -p rootdir

finalTest: finalTest.c
	gcc finalTest.c -o finalTest

distdir:
	cp Makefile $(distdir)

mostlyclean clean distclean mainainer-clean:
	rm -r bbfs.log mountdir rootdir .Storage finalTest
