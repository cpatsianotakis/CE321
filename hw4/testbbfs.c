#include <stdio.h>
#include <string.h>

#define  LOW_BASE

int main (int argc, char *argv[] )
{
	
	FILE *file;

	#ifndef LOW_BASE 
	//8192 bytes string
	char input[] = "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
JpbKFCjmHIr6bj2m1";
	#endif

	#ifdef LOW_BASE
	char str[100];
	char input[] = "AAAAABBBBBCCCCCDDDDDEEEEEFFFFFCCCCCGGGGGAAAAAHHHHH";
	#endif

	int len = strlen(input);

	// file = fopen ( "/home/ttsiapras/Desktop/fuse/example/mountdir/testFile.txt", "w+" );
	// if( file == NULL ) perror("Could not open file!\n");

	// if ( fwrite ( input, sizeof(char), len, file) == 0 ) perror ("Could not write to file!\n");
	// fflush(file);
	// fclose ( file);


	//test for overwritting by introducing offset to the write function
	file = fopen ( "/home/ttsiapras/Desktop/fuse/example/mountdir/testFile.txt", "r+" );
	if ( file == NULL ) perror("Could not open file!\n");
	
	#ifdef LOW_BASE
	fseek(file,10,SEEK_SET);
	
	if ( fwrite ( "111", sizeof(char), 3, file) == 0 ) perror ("Could not write to file!\n");
	fflush(file);
	fclose ( file);
	#endif
	
	#ifndef LOW_BASE
	fseek(file,4096,SEEK_SET);

	if ( fwrite ( "xxxxx", sizeof(char), 5, file) == 0 ) perror ("Could not write to file!\n");
	fflush(file);
	fclose ( file);
	#endif
	

	

	file = fopen ( "/home/ttsiapras/Desktop/fuse/example/mountdir/testFile.txt", "r+" );
	fread(str,sizeof(char),len,file);
	fclose ( file);
	str[len] = '\0';
	printf("->%s<-\n",str);

	return 0;
}