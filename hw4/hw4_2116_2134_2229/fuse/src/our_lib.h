/* Return the offset in the compresed file */
int getNewOffset( int offset, int base );

/* Return the size of the Input */
int getSize( char* input );

/* Adds the Hash to the Hash ledger and return the ID that was assigned */
int addHash( FILE *hash_file, int hash_file_size, char *new_hash,FILE * diagn_fd );

/* Creates the hidden file that contains the actual Block data */
void createBlock( char* block, int id,char* dirPath ,FILE * diagn_fd );

/* Tries to find the hash and in case of failure creates the block */
int compareHash( int *isNewBlock, FILE *hash_file, char* in_hash,FILE * diagn_fd );

/* Returns the 20-character long Hash of the Input given */
char * getHash( char* input );

/* Starts the procces of finding/comparing/creating/storing the block */
/* Returns the IDs of the identical existing or newly created BLOCK   */
int PushBlock( char* block,FILE *hash_file,char* dirPath, FILE * diagn_fd );

/* Takes an Array of bytes, breaks it into block sized segments */
/* and callsthe PushBlock for each of the segments              */
char* compressBuffer(const char* buf, int size, FILE* hash_file, char* dirPath, int base );

/* Returns the contents of the block specified by the ID given */
char *readBlock( char *dirPath, int id, int base, FILE * diagn_fd );

/* Returns the reconstructed file */
char *readFile( char *dirPath, size_t size, off_t offset, FILE *file, int base, char* fpath );

/* Returns the IDs of the blocks thst got overwritten in order to be removed */
char* getOverwrittenBlocks( char* path, int size, int offset, int base );

/* Receives a sting like "XXX\nYYY\nOOO\n" representing IDs and removes them */
int removeBlocks( char* input, FILE *hash_file,char* dirPath );

/* Returns the actual size of a file in mountdir through reconstruction */
int findRealSize( char *dirPath, size_t size, char *fpath, int base );