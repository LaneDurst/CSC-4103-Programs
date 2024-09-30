/////////////////////////////////////////////////////////////////////
// Intentionally flawed system call library that (doesn't really)
// implement "safe" file I/O.  The intention is to disallow writing
// "\xFE\xED\xFA\xCE" and "\xFE\xED\xFA\xCF" at the beginning of a
// file.  Written by Golden G. Richard III (@nolaforensix).  Props to
// Brian Hay for the idea, based on a similar exercise he used in a
// training exercise.
/////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// filesystem error code
typedef enum  {
  NONE, 
  OPEN_FAILED,
  CLOSE_FAILED,
  READ_FAILED,
  WRITE_FAILED,
  ILLEGAL_MACHO
} FSError;

typedef struct _File
{
  FILE *fp;
  unsigned char mem[4];
} _File;

// file handle type           // <---- YOU WILL HAVE TO MODIFY THIS TYPE! Hint: File should become
typedef _File* File;           // <---- a pointer to a structure type that contains the FILE * field 
                              // <---- and some other stuff
// seek anchors
typedef enum {
  BEGINNING_OF_FILE, 
  CURRENT_POSITION, 
  END_OF_FILE
} SeekAnchor;

// function prototypes for system calls provided by "fileio.c"

// opens file with pathname 'name'. Files are always opened for
// read/write access.  If the open fails for any reason, the global
// 'fserror' is set to OPEN_FAILED, otherwise to NONE.
File open_file(char *name);        // <---- YOU WILL HAVE TO MODIFY THIS FUNCTION!

// closes file with handle 'file'. If the close fails for any reason,
// the global 'fserror' is set to CLOSE_FAILED, otherwise to NONE.
void close_file(File file);

// reads 'num_bytes' into 'data' from 'file' at 'offset' bytes from a
// particular 'start'-ing position.  Returns the number of bytes
// read. If the read fails for any reason, the global 'fserror' is set
// to READ_FAILED, otherwise to NONE.
unsigned long read_file_from(File file,
			     void *data,
			     unsigned long num_bytes,
			     SeekAnchor start,
			     long offset);

// writes 'num_bytes' from 'data' into 'file' at 'offset' bytes from a
// particular 'start'-ing position. Returns the number of bytes
// written.  Disallows writing "\xFE\xED\xFA\xCE" or
// "\xFE\xED\xFA\xCF" at the beginning of a file.  If an attempt is
// made to modify a file such that one of the forbidden strings would
// appear in the first four bytes of the file, the write operation
// fails and ILLEGAL_MACHO is stored in the global 'fserror'.  If the
// write fails for any other reason, 'fserror' is set to WRITE_FAILED,
// otherwise to NONE.
unsigned long write_file_at(File file,
			    void *data,
			    unsigned long num_bytes, 
			    SeekAnchor start,
			    long offset);         // <---- YOU WILL HAVE TO MODIFY THIS FUNCTION!

// describes current filesystem error code 
void fs_print_error(void);

// GLOBALS //

// filesystem error code set (set by each function)
extern FSError fserror;

