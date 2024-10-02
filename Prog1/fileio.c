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

  if (! file.fp || (start != BEGINNING_OF_FILE && 
		 start != CURRENT_POSITION && start != END_OF_FILE)) {
    return false;
  }
  else {
    if (! fseek(file.fp, offset, start == BEGINNING_OF_FILE ? SEEK_SET : 
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
  
  File file;

  file.mem = malloc(5*sizeof(unsigned char));

  // HINT:  Your new implementation probably needs a call to malloc()!
  
  fserror = NONE;
  
  // try to open existing file
  file.fp = fopen(name, "r+");
  if (! file.fp) 
  {
    // fail, fall back to creation
    file.fp = fopen(name, "w+");
    if (! file.fp) 
    {
      fserror = OPEN_FAILED;
    }
  }

  return file;
}

// closes file with handle 'file'. If the close fails for any reason,
// the global 'fserror' is set to CLOSE_FAILED, otherwise to NONE.
void close_file(File file) {

  free(file.mem);
  if (file.fp && ! fclose(file.fp)) {
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

  if (! file.fp || ! seek_file(file, start, offset)) {
    fserror = READ_FAILED;
  }
  else {
    bytes_read = fread(data, 1, num_bytes, file.fp);
    if (ferror(file.fp)) {
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
unsigned long write_file_at(File file, void *data, unsigned long num_bytes, SeekAnchor start, long offset) 
{

  //appending up to 4 characters already in the file to mem
  int ch;
  int count = 0;
  fseek(file.fp, 0, SEEK_SET);
  while(((ch=getc(file.fp)) != EOF) && count < 4)
  {
    file.mem[count] = ch;
    count++;
  }
  file.mem[count] = '\0';
  
  fseek(file.fp, 0, SEEK_SET);


  unsigned char *d = malloc(4*sizeof(unsigned char));
  unsigned long bytes_written = 0L;

  //if mem is empty just copy the contents of null
  if (file.mem[0] == '\0')
  {
    printf("[DEBUG] Memory does not contain any info:\n");

    printf("[DEBUG] data value is: ");
    for (int i = 0; i<sizeof(data); i++)
    {
        printf("%02x ", ((unsigned char *)data)[i]);
        //printf("%02x [%02d] ", ((unsigned char *)data)[i], ((unsigned char*) data)[i]);
        if(((unsigned char *)data)[i] == '\0') break;
    }
    printf("\n");


    strcpy(d, (unsigned char *) data);

    printf("[DEBUG] d-value is: ");
    for (int i = 0; i<sizeof(unsigned long); i++)
    {
        printf("%02x ", ((unsigned char *)d)[i]);
        //printf("%02x [%02d] ", ((unsigned char *)d)[i], ((unsigned char *)d)[i]);
        if(((unsigned char *)d)[i] == '\0') break;
    }
    printf("\n");
  }
  else
  {
    printf("[DEBUG] mem value is: ");
    for (int i = 0; i<sizeof(file.mem); i++)
    {
        printf("%02x ", ((unsigned char *)file.mem)[i]);
        //printf("%02x [%02d] ", ((unsigned char *)file.mem)[i], ((unsigned char*)file.mem)[i]);
        if(((unsigned char *)file.mem)[i] == '\0') break;
    }
    printf("\n");

    printf("[DEBUG] data value is: ");
    for (int i = 0; i<sizeof(data); i++)
    {
        printf("%02x ", ((unsigned char *)data)[i]);
        //printf("%02x [%02d] ", ((unsigned char *)data)[i], ((unsigned char*) data)[i]);
        if(((unsigned char *)data)[i] == '\0') break;
    }
    printf("\n");

    strcpy(d, (unsigned char*) file.mem);
    strcat(d, (unsigned char *) data);

    printf("[DEBUG] d-value is: ");
    for (int i = 0; i<sizeof(unsigned long); i++)
    {
        printf("%02x ", ((unsigned char *)d)[i]);
        //printf("%02x [%02d] ", ((unsigned char *)d)[i], ((unsigned char *)d)[i]);
        if(((unsigned char *)d)[i] == '\0') break;
    }
    printf("\n");
  }



  fserror = NONE;
  if (! file.fp || ! seek_file(file, start, offset)) {fserror = WRITE_FAILED;}
  else if (offset == 0L &&
	   strlen(d) >= 4 &&
	   d[0] == 0xFE && 
	   d[1] == 0xED &&
	   d[2] == 0xFA &&
	   (d[3] == 0xCE || d[3] == 0xCF)) {
    // don't let 0xfeedface or 0xfeedfacf ever appear at the
    // beginning of the file!  IT CAN'T BE THIS EASY, CAN IT?
    fserror = ILLEGAL_MACHO;
  }
  else {
    bytes_written = fwrite(data, 1, num_bytes, file.fp);
    if (bytes_written < num_bytes) {fserror = WRITE_FAILED;}
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

