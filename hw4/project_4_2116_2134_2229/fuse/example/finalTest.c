#include <stdio.h>
#include <string.h>

#define BLOCK_SIZE 4096


//CAUTION! 
//Start with WRITE_FILE and then activate only one of the 
//rest options to test the functionality of our file system

#define WRITE_FILE 1
#define OVERWRITE_FILE 0
#define READ_FILE 0
#define DELETE_FILE 0

int main (int argc, char *argv[] )
{
	int i;
	FILE *file;
	char randomBytes[BLOCK_SIZE * 10]; //10 Blocks

	randomBytes[0] = 'A' + (srandom(176) % 26 );

	for ( i = 0; i < BLOCK_SIZE * 10; i++)
		randomBytes[i] = 'A' + (random() % 26 );


#if (WRITE_FILE == 1)
	
	mkdir("mountdir/testDirectory/", 0700);

	file = fopen ( "mountdir/testDirectory/testFile1.txt", "w+" );
	if( file == NULL ) 
		file = fopen ( "mountdir/testDirectory/testFile1.txt", "r+" );

	if ( fwrite ( randomBytes, sizeof(char), BLOCK_SIZE * 10, file) == 0 ) 
		perror ("Could not write to file!\n");

	fflush(file);
	fclose ( file);

	for ( i = 0; i < BLOCK_SIZE * 2; i++)
		randomBytes[i] = 'A' + (random() % 26 );


	file = fopen ( "mountdir/testFile2.txt", "w+" );
	if( file == NULL ) 
		file = fopen ( "mountdir/testFile2.txt", "r+" );

	if ( fwrite ( randomBytes, sizeof(char), BLOCK_SIZE * 10, file) == 0 ) 
		perror ("Could not write to file!\n");

	fflush(file);
	fclose (file);

#elif (OVERWRITE_FILE == 1)

	char overwriteStr[BLOCK_SIZE * 2]; //2  Blocks

	for ( i = 0; i < BLOCK_SIZE * 2; i++)
		overwriteStr[i] = randomBytes[i];

	file = fopen ( "mountdir/testFile2.txt", "r+" );
	if( file == NULL ) {
		printf("Error! File not found\n");
		return 1;
	}
		
	
	fseek(file, BLOCK_SIZE * 2, SEEK_SET); //We set the offset at two blocks
	if ( fwrite ( overwriteStr, sizeof(char), BLOCK_SIZE * 2, file) == 0 ) 
		perror ("Could not write to file!\n");

	fflush(file);
	fclose (file);

#elif (READ_FILE == 1)

	char readStr1[BLOCK_SIZE * 10];
	char readStr2[BLOCK_SIZE * 3];
	FILE *readFile;


	//Reading 10 blocks from file testFile1.txt and exporting result to file readFile1.txt

	file = fopen ( "mountdir/testDirectory/testFile1.txt", "r+" );
	if( file == NULL ) {
		printf("Error! File not found #1\n");
		return 1;
	} 
	
	readFile = fopen ( "readFile1.txt", "w+" );
	if( readFile == NULL ) {
		printf("Error! File not found #2\n");
		return 1;
	}

	fseek(file, 0, SEEK_SET);
	fread(readStr1, sizeof(char), BLOCK_SIZE * 10, file);


	if ( fwrite(readStr1, sizeof(char), BLOCK_SIZE * 10, readFile) == 0 )
		perror ("Could not write to file!\n");

	fclose(readFile);


	//Reading 2 blocks with an offset of 3 blocks from file testFile2.txt 
	//and exporting result to file readFile2.txt

	file = fopen ( "mountdir/testFile2.txt", "r+" );
	if( file == NULL ) {
		printf("Error! File not found #3\n");
		return 1;
	} 

	readFile = fopen ( "readFile2.txt", "w+" );
	if( readFile == NULL ) {
		printf("Error! File not found #4\n");
		return 1;
	}

	fseek(file, BLOCK_SIZE * 3, SEEK_SET);
	fread(readStr2, sizeof(char), BLOCK_SIZE * 2, file);

	if ( fwrite(readStr2, sizeof(char), BLOCK_SIZE * 2, readFile) == 0 )
		perror ("Could not write to file!\n");


	fclose(readFile);
	fclose (file);

#elif (DELETE_FILE == 1)

	remove("mountdir/testFile2.txt");
 	remove("mountdir/testDirectory/testFile1.txt");


#endif


}






