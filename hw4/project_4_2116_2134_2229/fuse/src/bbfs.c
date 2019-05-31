#include "config.h"
#include "params.h"
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <libgen.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <openssl/sha.h>
#include <sys/stat.h>

#ifdef HAVE_SYS_XATTR_H
#include <sys/xattr.h>
#endif

#include "log.h"
#include "our_lib.h"

#define BASE 4096

static void bb_fullpath(char fpath[PATH_MAX], const char *path)
{
    strcpy(fpath, BB_DATA->rootdir);
    strncat(fpath, path, PATH_MAX);
}

static void StoragePath(char fpath[PATH_MAX], const char *path)
{
    strcpy(fpath, BB_DATA->hiddendir);
    strncat(fpath, path, PATH_MAX);
}

//##################################################################//
//                         ALTERED FUNCTION                         //
//##################################################################//

int bb_getattr(const char *path, struct stat *statbuf)
{
    int retstat;
    char fpath[PATH_MAX]; //full path of the file
    int totalBytes = 0;
    char *ret; 
    log_msg("\nbb_getattr(path=\"%s\", statbuf=0x%08x)\n", path, statbuf);
    
    bb_fullpath(fpath, path);

    retstat = log_syscall("lstat", lstat(fpath, statbuf), 0);

    totalBytes = findRealSize (BB_DATA->hiddendir, statbuf->st_size, fpath, BASE);

    if ( totalBytes > 0 ) {
        statbuf->st_size = totalBytes;
        log_msg("\nReal Bytes of File = %d\n", totalBytes );
    }
    log_stat(statbuf);
    return retstat;
}
int bb_readlink(const char *path, char *link, size_t size)
{
    int retstat;
    char fpath[PATH_MAX];
    
    log_msg("\nbb_readlink(path=\"%s\", link=\"%s\", size=%d)\n",
	  path, link, size);
    bb_fullpath(fpath, path);

    retstat = log_syscall("readlink", readlink(fpath, link, size - 1), 0);
    if (retstat >= 0) {
	link[retstat] = '\0';
	retstat = 0;
	log_msg("    link=\"%s\"\n", link);
    }
    
    return retstat;
}

int bb_mknod(const char *path, mode_t mode, dev_t dev)
{
    int retstat;
    char fpath[PATH_MAX];
    
    log_msg("\nbb_mknod(path=\"%s\", mode=0%3o, dev=%lld)\n",path, mode, dev);
    bb_fullpath(fpath, path);
    
    // On Linux this could just be 'mknod(path, mode, dev)' but this
    // tries to be be more portable by honoring the quote in the Linux
    // mknod man page stating the only portable use of mknod() is to
    // make a fifo, but saying it should never actually be used for
    // that.
    if (S_ISREG(mode)) {
	retstat = log_syscall("open", open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode), 0);
	if (retstat >= 0)
	    retstat = log_syscall("close", close(retstat), 0);
    } else
	if (S_ISFIFO(mode))
	    retstat = log_syscall("mkfifo", mkfifo(fpath, mode), 0);
	else
	    retstat = log_syscall("mknod", mknod(fpath, mode, dev), 0);
    
    return retstat;
}

int bb_mkdir(const char *path, mode_t mode)
{
    char fpath[PATH_MAX];
    
    log_msg("\nbb_mkdir(path=\"%s\", mode=0%3o)\n",
	    path, mode);
    bb_fullpath(fpath, path);

    return log_syscall("mkdir", mkdir(fpath, mode), 0);
}

//##################################################################//
//                         ALTERED FUNCTION                         //
//##################################################################//

int bb_unlink(const char *path)
{
    char fpath[PATH_MAX];
    int fileSize;
    FILE* fd;
    char* IDsToRemove;
    
    log_msg("bb_unlink(path=\"%s\")\n",
	    path);
    bb_fullpath(fpath, path);

    //#################### OUR LINES ###################//
    fd = fopen(fpath,"r+");
    fseek (fd, 0, SEEK_END);//find the EOF offset
    fileSize = ftell( fd );
    fclose(fd);
    IDsToRemove = getOverwrittenBlocks(fpath, (fileSize/4)*BASE, 0, BASE);
    log_msg("...IDs to remove=->%s<-...\n", IDsToRemove);
    removeBlocks( IDsToRemove, BB_DATA->hashLedger, BB_DATA->hiddendir);
    //#################### OUR LINES ###################//

    return log_syscall("unlink", unlink(fpath), 0);
}
/** Remove a directory */
int bb_rmdir(const char *path)
{
    char fpath[PATH_MAX];
    
    log_msg("bb_rmdir(path=\"%s\")\n",
	    path);
    bb_fullpath(fpath, path);

    return log_syscall("rmdir", rmdir(fpath), 0);
}

int bb_symlink(const char *path, const char *link)
{
    char flink[PATH_MAX];
    
    log_msg("\nbb_symlink(path=\"%s\", link=\"%s\")\n",
	    path, link);
    bb_fullpath(flink, link);

    return log_syscall("symlink", symlink(path, flink), 0);
}

/** Rename a file */
int bb_rename(const char *path, const char *newpath)
{
    char fpath[PATH_MAX];
    char fnewpath[PATH_MAX];
    
    log_msg("\nbb_rename(fpath=\"%s\", newpath=\"%s\")\n",
	    path, newpath);
    bb_fullpath(fpath, path);
    bb_fullpath(fnewpath, newpath);

    return log_syscall("rename", rename(fpath, fnewpath), 0);
}

int bb_link(const char *path, const char *newpath)
{
    char fpath[PATH_MAX], fnewpath[PATH_MAX];
    
    log_msg("\nbb_link(path=\"%s\", newpath=\"%s\")\n",
	    path, newpath);
    bb_fullpath(fpath, path);
    bb_fullpath(fnewpath, newpath);

    return log_syscall("link", link(fpath, fnewpath), 0);
}
int bb_chmod(const char *path, mode_t mode)
{
    char fpath[PATH_MAX];
    
    log_msg("\nbb_chmod(fpath=\"%s\", mode=0%03o)\n",
	    path, mode);
    bb_fullpath(fpath, path);

    return log_syscall("chmod", chmod(fpath, mode), 0);
}
int bb_chown(const char *path, uid_t uid, gid_t gid) 
{
    char fpath[PATH_MAX];
    
    log_msg("\nbb_chown(path=\"%s\", uid=%d, gid=%d)\n",
	    path, uid, gid);
    bb_fullpath(fpath, path);

    return log_syscall("chown", chown(fpath, uid, gid), 0);
}
int bb_truncate(const char *path, off_t newsize)
{
    char fpath[PATH_MAX];
    
    log_msg("\nbb_truncate(path=\"%s\", newsize=%lld)\n",
	    path, newsize);
    bb_fullpath(fpath, path);

    //return log_syscall("truncate", truncate(fpath, newsize), 0);
    return 0;
}
int bb_utime(const char *path, struct utimbuf *ubuf)
{
    char fpath[PATH_MAX];
    
    log_msg("\nbb_utime(path=\"%s\", ubuf=0x%08x)\n",
	    path, ubuf);
    bb_fullpath(fpath, path);

    return log_syscall("utime", utime(fpath, ubuf), 0);
}

//##################################################################//
//                         ALTERED FUNCTION                         //
//##################################################################//

int bb_open(const char *path, struct fuse_file_info *fi)
{
    int retstat = 0;
    int fd;
    char fpath[PATH_MAX];
    
    log_msg("\nbb_open(path\"%s\", fi=0x%08x)\n----------\n",
	    path, fi);
    bb_fullpath(fpath, path);
    
    // if the open call succeeds, my retstat is the file descriptor,
    // else it's -errno.  I'm making sure that in that case the saved
    // file descriptor is exactly -1.
    fd = log_syscall("open", open(fpath, fi->flags), 0);
    if (fd < 0)
	retstat = log_error("open");
	
    fi->fh = fd;

    //log_fi(fi);
    
    return retstat;
}

//##################################################################//
//                         ALTERED FUNCTION                         //
//##################################################################//

int bb_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    int retstat = 0;
    char fpath[PATH_MAX];
    bb_fullpath(fpath, path);
    
    FILE *tempFileToRead;
    FILE *fileIDs;

    char *fileData;
    char tempFilePath[PATH_MAX]; 

    int sizeOfIDs;
    int dataSize;
    int bytesRead;
    int tempFD;
    int removeResult;

    fileIDs = fopen(fpath,"r+");

    fseek (fileIDs, 0, SEEK_END);             //find the EOF offset
    sizeOfIDs = ftell( fileIDs );
    fseek (fileIDs, 0, SEEK_SET);
    
    log_msg("\nSize of IDs = %d\n", sizeOfIDs); 
    log_msg("\nbb_read(path=\"%s\", buf=0x%08x, size=%d, offset=%lld, fi=0x%08x)\n",
        path, buf, size, offset, fi);

    fileData = readFile(BB_DATA->hiddendir, sizeOfIDs, offset, fileIDs, BASE, fpath);
    
    if (fileData == NULL) 
    {
        log_msg("\nNULL! <--> No blocks found for this file!\n" );
        bytesRead = log_syscall("pread", pread(fi->fh, buf, size, offset), 0);
    }
    else 
    {
        dataSize = strlen(fileData);
        log_msg("\nSize = %d, Data: %s\n", dataSize, fileData);

        sprintf ( tempFilePath, "%s/temp\n", BB_DATA->hiddendir);
        tempFileToRead = fopen(tempFilePath, "w+");

        tempFD = fileno(tempFileToRead);

        fwrite ( fileData, sizeof(char), dataSize, tempFileToRead );
        rewind(tempFileToRead);
        fflush(tempFileToRead);

        bytesRead =  log_syscall("pread", pread(tempFD, buf, size, 0), 0);
        
        fclose(tempFileToRead);

        removeResult = remove(tempFilePath);
        log_msg("\nremove() returned %d\n", removeResult);

    }
    

    log_msg("\nBB_READ RETURNED\n----------\n");
    fclose(fileIDs);

    return bytesRead;
}

//##################################################################//
//                         ALTERED FUNCTION                         //
//##################################################################//
int bb_write(const char *path, const char *buf, size_t size, off_t offset,struct fuse_file_info *fi)
{   
    int retstat = 0;
    char fpath[PATH_MAX];
    bb_fullpath(fpath, path);
    char* compressedBuf;
    char* IDsToRemove;
    int compressedOffset=0;
    int compressedSize=9;
    int i;
    
    log_msg("\nbb_write(path=\"%s\", buf=0x%08x, size=%d, offset=%lld, fi=0x%08x)\n",path, buf, size, offset, fi);

    //###################################OUR LINES###########################################//
    log_msg("||buf=->%s<-, size=%d, offset=%lld||\n", buf, size, offset);                    

    IDsToRemove = getOverwrittenBlocks(fpath, size, offset, BASE);
    removeBlocks( IDsToRemove, BB_DATA->hashLedger, BB_DATA->hiddendir);
    log_msg("...IDs to remove=->%s<-...\n", IDsToRemove);

    compressedBuf    = compressBuffer(buf, size, BB_DATA->hashLedger, BB_DATA->hiddendir, BASE);
    compressedOffset = getNewOffset(offset, BASE);
    compressedSize   = getSize(compressedBuf);
    //compressdBuf's form is XXX\nYYY\nZZZ\n..... with the appropriate IDs
    //####################################################################################//
    retstat = pwrite(fi->fh, compressedBuf, compressedSize, compressedOffset);

    return size;
}

int bb_statfs(const char *path, struct statvfs *statv)
{
    int retstat = 0;
    char fpath[PATH_MAX];
    
    log_msg("\nbb_statfs(path=\"%s\", statv=0x%08x)\n",
	    path, statv);
    bb_fullpath(fpath, path);
    
    // get stats for underlying filesystem
    retstat = log_syscall("statvfs", statvfs(fpath, statv), 0);
    
    log_statvfs(statv);
    
    return retstat;
}
int bb_flush(const char *path, struct fuse_file_info *fi)
{
    log_msg("\nbb_flush(path=\"%s\", fi=0x%08x)\n----------\n", path, fi);
    // no need to get fpath on this one, since I work from fi->fh not the path
    //log_fi(fi);
    return 0;
}
int bb_release(const char *path, struct fuse_file_info *fi)
{
    log_msg("\nbb_release(path=\"%s\", fi=0x%08x)\n----------\n",path, fi);
    //log_fi(fi);

    // We need to close the file.  Had we allocated any resources
    // (buffers etc) we'd need to free them here as well.
    return log_syscall("close", close(fi->fh), 0);
}
int bb_fsync(const char *path, int datasync, struct fuse_file_info *fi)
{
    log_msg("\nbb_fsync(path=\"%s\", datasync=%d, fi=0x%08x)\n",
	    path, datasync, fi);
    log_fi(fi);
    
    // some unix-like systems (notably freebsd) don't have a datasync call
#ifdef HAVE_FDATASYNC
    if (datasync)
	return log_syscall("fdatasync", fdatasync(fi->fh), 0);
    else
#endif	
	return log_syscall("fsync", fsync(fi->fh), 0);
}

#ifdef HAVE_SYS_XATTR_H
/** Note that my implementations of the various xattr functions use
the 'l-' versions of the functions (eg bb_setxattr() calls
lsetxattr() not setxattr(), etc).  This is because it appears any
symbolic links are resolved before the actual call takes place, so
I only need to use the system-provided calls that don't follow
them 
*/

int bb_setxattr(const char *path, const char *name, const char *value, size_t size, int flags)
{
    char fpath[PATH_MAX];
    
    log_msg("\nbb_setxattr(path=\"%s\", name=\"%s\", value=\"%s\", size=%d, flags=0x%08x)\n",
	    path, name, value, size, flags);
    bb_fullpath(fpath, path);

    return log_syscall("lsetxattr", lsetxattr(fpath, name, value, size, flags), 0);
}
int bb_getxattr(const char *path, const char *name, char *value, size_t size)
{
    int retstat = 0;
    char fpath[PATH_MAX];
    
    log_msg("\nbb_getxattr(path = \"%s\", name = \"%s\", value = 0x%08x, size = %d)\n",
	    path, name, value, size);
    bb_fullpath(fpath, path);

    retstat = log_syscall("lgetxattr", lgetxattr(fpath, name, value, size), 0);
    if (retstat >= 0)
	log_msg("    value = \"%s\"\n", value);
    
    return retstat;
}
int bb_listxattr(const char *path, char *list, size_t size)
{
    int retstat = 0;
    char fpath[PATH_MAX];
    char *ptr;
    
    log_msg("\nbb_listxattr(path=\"%s\", list=0x%08x, size=%d)\n",
	    path, list, size
	    );
    bb_fullpath(fpath, path);

    retstat = log_syscall("llistxattr", llistxattr(fpath, list, size), 0);
    if (retstat >= 0) {
	log_msg("    returned attributes (length %d):\n", retstat);
	if (list != NULL)
	    for (ptr = list; ptr < list + retstat; ptr += strlen(ptr)+1)
		log_msg("    \"%s\"\n", ptr);
	else
	    log_msg("    (null)\n");
    }
    
    return retstat;
}
int bb_removexattr(const char *path, const char *name)
{
    char fpath[PATH_MAX];
    
    log_msg("\nbb_removexattr(path=\"%s\", name=\"%s\")\n",
	    path, name);
    bb_fullpath(fpath, path);

    return log_syscall("lremovexattr", lremovexattr(fpath, name), 0);
}
#endif

/** Open directory
 *
 * This method should check if the open operation is permitted for
 * this  directory
 *
 * Introduced in version 2.3
 */
int bb_opendir(const char *path, struct fuse_file_info *fi)
{
    DIR *dp;
    int retstat = 0;
    char fpath[PATH_MAX];
    
    log_msg("\nbb_opendir(path=\"%s\", fi=0x%08x)\n----------\n",
	  path, fi);
    bb_fullpath(fpath, path);

    // since opendir returns a pointer, takes some custom handling of
    // return status.
    dp = opendir(fpath);
    log_msg("    opendir returned 0x%p\n", dp);
    if (dp == NULL)
	retstat = log_error("bb_opendir opendir");
    
    fi->fh = (intptr_t) dp;
    
    //log_fi(fi);
    
    return retstat;
}

int bb_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset,struct fuse_file_info *fi)
{
    int retstat = 0;
    DIR *dp;
    struct dirent *de;
    
    log_msg("\nbb_readdir(path=\"%s\", buf=0x%08x, filler=0x%08x, offset=%lld, fi=0x%08x)\n----------\n",
	    path, buf, filler, offset, fi);
    // once again, no need for fullpath -- but note that I need to cast fi->fh
    dp = (DIR *) (uintptr_t) fi->fh;

    // Every directory contains at least two entries: . and ..  If my
    // first call to the system readdir() returns NULL I've got an
    // error; near as I can tell, that's the only condition under
    // which I can get an error from readdir()
    de = readdir(dp);
    log_msg("    readdir returned 0x%p\n", de);
    if (de == 0) {
    	retstat = log_error("bb_readdir readdir");
    	return retstat;
    }

    // This will copy the entire directory into the buffer.  The loop exits
    // when either the system readdir() returns NULL, or filler()
    // returns something non-zero.  The first case just means I've
    // read the whole directory; the second means the buffer is full.
    do {
    	log_msg("calling filler with name %s\n", de->d_name);
    	if (filler(buf, de->d_name, NULL, 0) != 0) {
    	    log_msg("    ERROR bb_readdir filler:  buffer full");
    	    return -ENOMEM;
    	}
    } while ((de = readdir(dp)) != NULL);
    
    //log_fi(fi);
    
    return retstat;
}

/** Release directory
 *
 * Introduced in version 2.3
 */
int bb_releasedir(const char *path, struct fuse_file_info *fi)
{
    int retstat = 0;
    
    log_msg("\nbb_releasedir(path=\"%s\", fi=0x%08x)\n----------\n",
	    path, fi);
    //log_fi(fi);
    
    closedir((DIR *) (uintptr_t) fi->fh);
    
    return retstat;
}

int bb_fsyncdir(const char *path, int datasync, struct fuse_file_info *fi)
{
    int retstat = 0;
    
    log_msg("\nbb_fsyncdir(path=\"%s\", datasync=%d, fi=0x%08x)\n",
	    path, datasync, fi);
    log_fi(fi);
    
    return retstat;
}

//##################################################################//
//                         ALTERED FUNCTION                         //
//##################################################################//

void *bb_init(struct fuse_conn_info *conn)
{
    //initialize the hash_ledger
    char str_to_hash_file[] = "001, 00000000000000000000, 00\n";
    fwrite ( str_to_hash_file, 30 ,1 , BB_DATA->hashLedger );
    fflush(BB_DATA->hashLedger);

    log_msg("\nbb_init()\n");
    
    log_conn(conn);
    log_fuse_context(fuse_get_context());
    
    return BB_DATA;
}

void bb_destroy(void *userdata)
{
    
    log_msg("\nbb_destroy(userdata=0x%08x)\n", userdata);
}

int bb_access(const char *path, int mask)
{
    int retstat = 0;
    char fpath[PATH_MAX];
   
    log_msg("\nbb_access(path=\"%s\", mask=0%o)\n",
	    path, mask);
    bb_fullpath(fpath, path);
    
    retstat = access(fpath, mask);
    
    if (retstat < 0)
	retstat = log_error("bb_access access");
    
    return retstat;
}

int bb_ftruncate(const char *path, off_t offset, struct fuse_file_info *fi)
{
    int retstat = 0;
    
    log_msg("\nbb_ftruncate(path=\"%s\", offset=%lld, fi=0x%08x)\n",
	    path, offset, fi);
    log_fi(fi);
    
    retstat = ftruncate(fi->fh, offset);
    if (retstat < 0)
	retstat = log_error("bb_ftruncate ftruncate");
    
    return retstat;
}
int bb_fgetattr(const char *path, struct stat *statbuf, struct fuse_file_info *fi)
{
    int retstat = 0;
    
    log_msg("\nbb_fgetattr(path=\"%s\", statbuf=0x%08x, fi=0x%08x)\n",
	    path, statbuf, fi);
    log_fi(fi);

    if (!strcmp(path, "/"))
	return bb_getattr(path, statbuf);
    
    retstat = fstat(fi->fh, statbuf);
    if (retstat < 0)
	retstat = log_error("bb_fgetattr fstat");
    
    log_stat(statbuf);
    
    return retstat;
}

struct fuse_operations bb_oper = {
  .getattr = bb_getattr,
  .readlink = bb_readlink,
  // no .getdir -- that's deprecated
  .getdir = NULL,
  .mknod = bb_mknod,
  .mkdir = bb_mkdir,
  .unlink = bb_unlink,
  .rmdir = bb_rmdir,
  .symlink = bb_symlink,
  .rename = bb_rename,
  .link = bb_link,
  .chmod = bb_chmod,
  .chown = bb_chown,
  .truncate = bb_truncate,
  .utime = bb_utime,
  .open = bb_open,
  .read = bb_read,
  .write = bb_write,
  /** Just a placeholder, don't set */ // huh???
  .statfs = bb_statfs,
  .flush = bb_flush,
  .release = bb_release,
  .fsync = bb_fsync,
  
#ifdef HAVE_SYS_XATTR_H
  .setxattr = bb_setxattr,
  .getxattr = bb_getxattr,
  .listxattr = bb_listxattr,
  .removexattr = bb_removexattr,
#endif
  
  .opendir = bb_opendir,
  .readdir = bb_readdir,
  .releasedir = bb_releasedir,
  .fsyncdir = bb_fsyncdir,
  .init = bb_init,
  .destroy = bb_destroy,
  .access = bb_access,
  .ftruncate = bb_ftruncate,
  .fgetattr = bb_fgetattr
};

void bb_usage()
{
    fprintf(stderr, "usage:  bbfs [FUSE and mount options] rootDir mountPoint\n");
    abort();
}

//##################################################################//
//                         ALTERED FUNCTION                         //
//##################################################################//

int main(int argc, char *argv[])
{
    int fuse_stat;
    struct bb_state *bb_data;
    FILE *hash_file;

    if ((getuid() == 0) || (geteuid() == 0)) {
        fprintf(stderr, "Running BBFS as root opens unnacceptable security holes\n");
        return 1;
    }

    // See which version of fuse we're running
    fprintf(stderr, "Fuse library version %d.%d\n", FUSE_MAJOR_VERSION, FUSE_MINOR_VERSION);
    
    if ((argc < 3) || (argv[argc-2][0] == '-') || (argv[argc-1][0] == '-'))
    bb_usage();

    bb_data = malloc(sizeof(struct bb_state));
    if (bb_data == NULL) {
    perror("main calloc");
    abort();
    }

    // Pull the rootdir out of the argument list and save it in my
    // internal data
    bb_data->rootdir = realpath(argv[argc-2], NULL);
    argv[argc-2] = argv[argc-1];
    argv[argc-1] = NULL;
    argc--;
    
    bb_data->logfile = log_open();
    
    //#############################ADDED LINES###################################//
    mkdir(".Storage",0700);
    hash_file = fopen(".Storage/hash_ledger","wb+");

    if( fopen(".Storage/Diagnostics.txt","wb+") == NULL ) return -1;
    
    bb_data->hashLedger = hash_file;
    bb_data->hiddendir = realpath(".Storage", NULL);
    //###########################################################################//
    
    // turn over control to fuse
    fprintf(stderr, "about to call fuse_main\n");
    fuse_stat = fuse_main(argc, argv, &bb_oper, bb_data);
    fprintf(stderr, "fuse_main returned %d\n", fuse_stat);
    
    return fuse_stat;
}
