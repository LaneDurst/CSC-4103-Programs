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
#include "fileio.h"

//
// GLOBALS
//

FSError fserror;

//
// private functions
//

static bool seek_file(File file, SeekAnchor start, long offset) {
  
  if (! file || (start != BEGINNING_OF_FILE && 
		 start != CURRENT_POSITION && start != END_OF_FILE)) {
    return false;
  }
  else {
    if (! fseek(file, offset, start == BEGINNING_OF_FILE ? SEEK_SET : 
		(start == END_OF_FILE ? SEEK_END : SEEK_CUR))) {
      return true;
    }
    else {
      return false;
    }
  }
}

//
// public functions
//

// opens file with pathname 'name'. Files are always opened for
// read/write access.  If the open fails for any reason, the global
// 'fserror' is set to OPEN_FAILED, otherwise to NONE.
File open_file(char *name) {
  
  File fp;


  // HINT:  Your new implementation probably needs a call to malloc()!
  
  fserror = NONE;
  
  // try to open existing file
  fp = fopen(name, "r+");
  if (! fp) {
    // fail, fall back to creation
    fp = fopen(name, "w+");
    if (! fp) {
      fserror = OPEN_FAILED;
      return NULL;
    }
  }
  
  return fp;
}

// closes file with handle 'file'. If the close fails for any reason,
// the global 'fserror' is set to CLOSE_FAILED, otherwise to NONE.
void close_file(File file) {

  if (file && ! fclose(file)) {
    fserror=NONE;
  }
  else {
    fserror = CLOSE_FAILED;
  }
}

// reads 'num_bytes' into 'data' from 'file' at 'offset' bytes from a
// particular 'start'-ing position.  Returns the number of bytes
// read. If the read fails for any reason, the global 'fserror' is set
// to READ_FAILED, otherwise to NONE.
unsigned long read_file_from(File file, void *data, unsigned long num_bytes, 
			     SeekAnchor start, long offset) {

  unsigned long bytes_read = 0L;

  fserror = NONE;

  if (! file || ! seek_file(file, start, offset)) {
    fserror = READ_FAILED;
  }
  else {
    bytes_read = fread(data, 1, num_bytes, file);
    if (ferror(file)) {
      fserror = READ_FAILED;
    }
  }

  return bytes_read;
}

// writes 'num_bytes' from 'data' into 'file' at 'offset' bytes from a
// particular 'start'-ing position. Returns the number of bytes
// written.  Disallows writing "\xFE\xED\xFA\xCE" or
// "\xFE\xED\xFA\xCF" at the beginning of a file.  If an attempt is
// made to modify a file such that one of the forbidden strings would
// appear in the first four bytes of the file, the write operation
// fails and ILLEGAL_MACHO is stored in the global 'fserror'.  If the
// write fails for any other reason, 'fserror' is set to WRITE_FAILED,
// otherwise to NONE.
unsigned long write_file_at(File file, void *data, unsigned long num_bytes, 
			    SeekAnchor start, long offset) {
  
  unsigned long bytes_written = 0L;
  unsigned char *d = (unsigned char *)data;

  fserror = NONE;
  if (! file || ! seek_file(file, start, offset)) {
    fserror = WRITE_FAILED;
  }
  else if (offset == 0L &&
	   num_bytes >= 4 &&
	   d[0] == 0xFE &&
	   d[1] == 0xED &&
	   d[2] == 0xFA &&
	   (d[3] == 0xCE || d[3] == 0xCF)) {
    // don't let 0xfeedface or 0xfeedfacf ever appear at the
    // beginning of the file!  IT CAN'T BE THIS EASY, CAN IT?
    fserror = ILLEGAL_MACHO;
  }
  else {
    bytes_written = fwrite(data, 1, num_bytes, file);
    if (bytes_written < num_bytes) {
      fserror = WRITE_FAILED;
    }
  }
  return bytes_written;
}

// describes current filesystem error code 
void fs_print_error(void) {

  printf("FS ERROR: ");
  switch (fserror) {
  case NONE:
    puts("NONE");
    break;
  case OPEN_FAILED:
    puts("OPEN_FAILED");
    break;
  case CLOSE_FAILED:
    puts("CLOSE_FAILED");
    break;
  case READ_FAILED:
    puts("READ_FAILED");
    break;
  case WRITE_FAILED:
    puts("WRITE_FAILED");
    break;
  case ILLEGAL_MACHO:
    puts("ILLEGAL_MACHO: SHAME ON YOU!");
    break;
  default:
    puts("** UNKNOWN ERROR **");
  }
}

