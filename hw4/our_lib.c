#include <errno.h>
#include <fuse.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <openssl/sha.h>

# include "our_lib.h"

int getNewOffset( int offset, int base ){

    return((offset / base)*4); //the <*4> exosts because the ID has XXX\n form
}

int getSize( char* input ){

    return(strlen(input)); 
}

int addHash(FILE *hash_file, int hash_file_size, char *new_hash,FILE * diagn_fd ){
    int cur_position;
    int counter;
    int id;
    char id_char [4];
    char counter_char [3];
    char str_to_hash_file [30];
    
    fprintf(diagn_fd, "In addHash\n");                     //diagnostic MSG

    //####### FIND FIRST EMPTY COUNTER ########//
    fseek (hash_file, 0, SEEK_SET);
    do{
        fread (id_char, sizeof(char), 3, hash_file );      //reads the ID
        id_char[3] = '\0';
        id = atoi (id_char);
        
        fseek (hash_file, 25, SEEK_CUR );                  //passes the hash

        fread (counter_char, sizeof(char), 2, hash_file);  //reads the counter
        counter = atoi ( counter_char );

        //fseek (hash_file, 1, SEEK_CUR );                   // Go to next line 

        cur_position = ftell ( hash_file );                // Get current position
    } while( counter != 0 );    

    //####### ADD END LINE IF NEDDED ########//
    if( cur_position == hash_file_size ){
        sprintf( str_to_hash_file, "%03d, 00000000000000000000, 00\n" ,id + 1 );
        fwrite( str_to_hash_file, sizeof(char), 30, hash_file );
    }

    //####### ADD THE NEW HASH AND COUNT ########//
    fseek( hash_file, cur_position - 25, SEEK_SET );
    fwrite( new_hash, sizeof(char), 20, hash_file);
    fseek(hash_file, 2, SEEK_CUR);
    fwrite( "01", sizeof(char), 2, hash_file);
    fflush(hash_file);

    fprintf(diagn_fd, "Out addHash\n");                    //diagnostic MSG

    return id;
}

void create_block(char* block, int id,char* dirPath ,FILE * diagn_fd ){   
    int block_size = strlen( block );
    char block_file_name[100];
    FILE *NewBlock_file;
    

    fprintf(diagn_fd, "In create_block\n");                     //diagnostic MSG

    //############# CREATE NEW BLOCK FILE #############//
    sprintf( block_file_name, "%s/BL%03d",dirPath, id );      //synth the name
    NewBlock_file = fopen( block_file_name, "wb+" );           //open the new file


    fprintf(diagn_fd,"=========New File: >>");
    fprintf(diagn_fd,block_file_name);
    fprintf(diagn_fd,"<<\n");

    //############# WRITE THE BLOCK DATA #############//
    fwrite( block, sizeof(char), block_size, NewBlock_file );
    fclose( NewBlock_file );

    fprintf(diagn_fd, "Out create_block\n");                         //diagnostic MSG

    return;
}

int compareHash( int *isNewBlock, FILE *hash_file, char* in_hash,FILE * diagn_fd ){
    char check_hash [] = "00000000000000000000";
    char counter_char [3];
    char id_char [4];
    int cur_position;
    int found = 1;
    int file_size;
    int counter;
    int id;

    fprintf(diagn_fd, "In compareHash\n");       //diagnostic MSG

    *isNewBlock = 0;                             //flag for creating block

    
    fseek (hash_file, 0, SEEK_END);              //find the EOF offset
    file_size = ftell( hash_file );
    fseek (hash_file, 0, SEEK_SET);              //set offset to 0

    //############# SEARCH FOR FIRST IDENTICAL HASH #############//
    do {

        cur_position = ftell(hash_file);
        if ( cur_position == file_size )
        {   
            fseek (hash_file, 0, SEEK_SET);
            id = addHash ( hash_file, file_size, in_hash, diagn_fd);
            found = 0;
            *isNewBlock = 1;
            break;
        }

        fread ( id_char, sizeof(char), 3, hash_file );
        id_char[3] = '\0';
        id = atoi ( id_char );

        fseek ( hash_file, 2, SEEK_CUR );
        fread ( check_hash, sizeof(char), 20, hash_file );
        check_hash[20] = '\0';

        fseek( hash_file, 5,SEEK_CUR);

    } while (strcmp (check_hash, in_hash) != 0);

    //############# IF FOUND, INCREASE CNT & RETURN ID #############//
    if (found){
        // Read counter value //
        fseek( hash_file, -3,SEEK_CUR);
        fread (counter_char, sizeof(char), 2, hash_file);
        counter_char[2] = '\0';
        counter = atoi ( counter_char );

        // Check if new file block must be created //
        if ( counter == 0)
            *isNewBlock = 1;

        // Upgrade counter value //
        fseek( hash_file, -2,SEEK_CUR);

        fprintf(diagn_fd, "COUNTER:->>%d\n",counter);
        counter++;
        sprintf(counter_char,"%02d",counter);
        fwrite( counter_char, sizeof(char),2,hash_file);
        fflush(hash_file);
    }

    fprintf(diagn_fd, "Out compareHash\n");                     //diagnostic MSG

    return id;
}

char * getHash(char* input){
    int i = 0;
    unsigned char hash[20];
    char buf[20*2];
    char* output;
 
    output = malloc(sizeof(char)*21);
    memset(buf, 0x0, 20*2);
    memset(hash, 0x0, 20);
 
    SHA1((unsigned char *)input, strlen(input), hash);
 
    for (i=0; i < 20; i++) {
        sprintf((char*)&(buf[i*2]), "%02x", hash[i]);
    }
    for (i=0; i < 20; i++) {
        output[i] = buf[i];
    }
    output[20] = '\0';
    //printf("SHA1 of %s is %s\n", input, output);
 
    return output;
}

int PushBlock(char* block,FILE *hash_file,char* dirPath, FILE * diagn_fd){
    int res;
    int id = 0;
    int isNewBlock;
    char * hash;

    //included lines for diagnostics reasons
    fprintf(diagn_fd, "In Pushblock\n");                             //diagnostics

    hash = getHash(block);
    fprintf(diagn_fd,"=============Hash: >>");
    fprintf(diagn_fd,hash);
    fprintf(diagn_fd,"<<\n");

    id = compareHash( &isNewBlock, hash_file, hash, diagn_fd);

    if (isNewBlock){
        fprintf(diagn_fd,"==========Is New?: >> YES <<\n");
        create_block(block,id, dirPath, diagn_fd);
    }
    else{
        fprintf(diagn_fd,"==========Is New?: >> NO <<\n");
    }

    fprintf(diagn_fd, "Out Pushblock\n");                             //diagnostics
    return(id);
}

char* compressBuffer(char* buf, int size, FILE* hash_file, char* dirPath, int base){
    char* output;
    char block[base+1];
    char char_id[5];
    int nofBlocks = 0;
    int i,j,num_id;
    char fpath[200];

    //included lines for diagnostics reasons
    sprintf(fpath,"%s/Diagnostics.txt",dirPath);
    FILE * diagn_fd = fopen(fpath,"r+");
    fseek(diagn_fd, 0, SEEK_END);
    fprintf(diagn_fd, "------------\\/------------\n");                             //diagnostics
    fprintf(diagn_fd, "--------------------------\n");                              //diagnostics
    fprintf(diagn_fd, "In compressBuffer\n\n");                                     //diagnostics

    
    nofBlocks = size/ base;
    if((size % base)!= 0) nofBlocks++;

    output = malloc(sizeof(char) * (nofBlocks*4 + 1));
    output[0] = '\0';

    for ( i = 0; i < nofBlocks; i++ )
    {  
        for ( j = 0; j < base; j++)
            if(i*base + j < size) block[j] = buf[i*base + j];
            else break;

        block[j] = '\0';
        fprintf(diagn_fd,"==========Segment: >>");
        fprintf(diagn_fd,block);
        fprintf(diagn_fd,"<<\n");

        num_id = PushBlock(block,hash_file, dirPath, diagn_fd);

        sprintf(char_id, "%03d\n", num_id);

        fprintf(diagn_fd, "=========block ID: >> ");
        fprintf(diagn_fd,char_id);
        fprintf(diagn_fd, "\n\n");

        strcat(output,char_id);
    }

    fprintf(diagn_fd, "!!!\n");
    fprintf(diagn_fd, output);
    fprintf(diagn_fd, "!!!\n\n");
    fprintf(diagn_fd, "Out compressBuffer\n");
    fprintf(diagn_fd, "--------------------------\n");
    fclose(diagn_fd);

    return(output);
}

 
char *readBlock( char *dirPath, int id, int base, FILE * diagn_fd)
{
    FILE *block_file;
    char *block_contents;
    char *block_file_name;

    // Allocate memory for block file name with it's path //
    block_file_name = malloc ( sizeof (char) * (strlen (dirPath) + 7) );

    if ( block_file_name == NULL ) return NULL;

    // Write the block file name //
    sprintf ( block_file_name, "%s/BL%03d", dirPath, id );
    block_file = fopen ( block_file_name, "r");
    if ( block_file == NULL ){ 
        fprintf(diagn_fd, "Error reading block file!\n");
        return NULL;
    }
    // Allocate memory to get the whole block contents //
    block_contents = malloc (sizeof ( char ) * base + 1);
    if ( block_contents == NULL ) return NULL;

    // Get the block contents //
    fread ( block_contents, sizeof(char), base, block_file );
    block_contents [ base ] = '\0';

    // Delete the temp structures //
    fclose ( block_file );
    free ( block_file_name);

    // Return the final product //
    return (block_contents);
}

char *readFile( char *dirPath, size_t size, off_t offset, FILE *file, int base, char* fpath )
{
    int offset_block, no_blocks;
    int i;
    int id;

    char *read_result, *new_block;
    char id_char[5];

    sprintf(fpath,"%s/Diagnostics.txt",dirPath);
    FILE * diagn_fd = fopen(fpath,"r+");
    fseek(diagn_fd, 0, SEEK_END);
    fprintf(diagn_fd, "------------\\/------------\n");                             //diagnostics
    fprintf(diagn_fd, "--------------------------\n");                              //diagnostics
    fprintf(diagn_fd, "In readFile\n");                                             //diagnostics

    // Count into blocks //
    offset_block = getNewOffset ( offset, base );

    no_blocks = size / 4;

    //This if statement is need only if you want to read a block file inside mountdir/.Storage directory

    if ( no_blocks == 0 )
       return NULL;

    // Allocate return memory //
    read_result = (char *) malloc ( sizeof (char) * base * no_blocks  + 1 );
    if ( read_result == NULL )
        return NULL;
    
    // Set first byte of read_result as '\0' to help strcat funct //
    read_result[0] = '\0';

    // Get to offset position in the file //
    fseek ( file, 0, SEEK_SET );
    //for ( i = 0; i < offset_block; i++ )
    fseek ( file, offset_block, SEEK_CUR );

    // Get each block and put it to read_result //
    for ( i = 0; i < no_blocks; i++ )
    {
        fread ( id_char, sizeof(char), 4, file );
        id_char[4] = '\0';
        id = atoi ( id_char );

        new_block = readBlock (dirPath, id, base, diagn_fd);
        if ( new_block == NULL ) return NULL;

        strcat ( read_result, new_block );
        free ( new_block );
    }
    fprintf(diagn_fd, "Out readFile\n");                                      //diagnostics
    fprintf(diagn_fd, "--------------------------\n");                        //diagnostics
    fclose(diagn_fd);

    return (read_result);
}

int getNOFblocks( char* path, int sizeOfID ){
    FILE* fd = fopen(path, "r+");
    if(fd = NULL) return(-1);
    fseek(path, 0, SEEK_END);
    int size = ftell(path);
    return(size / sizeOfID);
}

char* getOverwrittenBlocks( char* path, int size, int offset, int base ){
    FILE * fd = fopen(path, "r+");
    char *str;
    int skip = getNewOffset(offset, base);
    int bytesRead = 0;

    

    str = malloc(sizeof(char)*(size/base)*4 +1);
    fseek(fd, skip, SEEK_SET);
    bytesRead = fread(str, sizeof(char),(size/base)*4, fd);
    str[bytesRead] = '\0';

    return(str);
}

int removeBlocks( char* input, FILE *hash_file,char* dirPath ){
    //input form: "003\n001\n003\n..."
    int counter;
    int IDToRemove;
    int i,j,nofIDs;
    char block_file_name[200];
    char counter_char[3];
    char tempStr[] = "000\0";

    nofIDs = strlen(input)/4 ;

    //#######     FIND ID IN HASH, DECREASE COUNTER     ########//
    //####### AND REMOVE FILE IF THE COUNTER BECOMES 00 ########//
    for ( j = 0; j < nofIDs; j++ ){                        //For all the IDs in the string
        
        for ( i = 0; i < 3; i++ ){                         //Get the individual ID
            tempStr[i] = input[j*4 + i];
            IDToRemove = atoi(tempStr);
        }

        fseek ( hash_file, 0, SEEK_SET );                  //reset the cursor
    
        for ( i = 1; i < IDToRemove; i++ )                 //skip lines until ID
            fseek( hash_file, 30, SEEK_CUR );
    
        
        fseek ( hash_file, 27, SEEK_CUR );
        fread ( counter_char, sizeof(char), 2, hash_file );// Get counter's value //
        counter_char[2] = '\0';
        counter = atoi ( counter_char );
    
        counter--;                                         // reduce counter //
    
        if(counter >= 0){                                  // Write new value of counter //
            sprintf( counter_char, "%02d", counter );
            fseek( hash_file, -2, SEEK_CUR );
            fwrite( counter_char, sizeof(char), 2, hash_file );
            fflush( hash_file );
        }

        // Delete file if there is no more links to this block //
        if ( counter == 0 )
        {
            sprintf( block_file_name, "%s/BL%03d",dirPath, IDToRemove );
            if ( remove ( block_file_name) == -1){
                return (1);
            }
        }
    }

    return 0;
}

int findRealSize( char *dirPath, size_t size, char *fpath, int base ) 
{
    FILE *fileIDs;
    int i;
    int sizeOfIDs;
    int numOfBlocks;
    int id;
    int totalBytes = 0;

    char id_char[5];
    char *block;

    sprintf(fpath,"%s/Diagnostics.txt",dirPath);
    FILE * diagn_fd = fopen(fpath,"r+");
    fseek(diagn_fd, 0, SEEK_END);

    fileIDs = fopen(fpath,"r+");

    if( fileIDs == NULL )
        return -1;

    fseek (fileIDs, 0, SEEK_END);             //find the EOF offset
    sizeOfIDs = ftell( fileIDs );
    fseek (fileIDs, 0, SEEK_SET);

    numOfBlocks = size / 4;

    for( i = 0; i < numOfBlocks; i++ )
    {
        fread( id_char, sizeof(char), 4, fileIDs );
        id_char[4] = '\0';
        id = atoi( id_char );

        block = readBlock(dirPath, id, base, diagn_fd);
        
        if( block == NULL ) 
            return -1;

        totalBytes += strlen(block);

        free( block );
    }

    return (totalBytes);
}
