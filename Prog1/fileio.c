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
//#include <strings.h>
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

// opens file with pathname 'name'. Files are always opened for
// read/write access.  If the open fails for any reason, the global
// 'fserror' is set to OPEN_FAILED, otherwise to NONE.
File open_file(char *name) {
  
  File file;
  file.mem = malloc(4*sizeof(unsigned char));
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
  read_file_from(file, file.mem, 4, SEEK_SET, 0L);

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

// Methods Added by Me //

void print_hex(void *data)
{
  for (int i = 0; i<sizeof(data); i++)
    {
        printf("%02x ", ((unsigned char *)data)[i]);
        //printf("%02x [%02d] ", ((unsigned char *)data)[i], ((unsigned char*) data)[i]);
        if(((unsigned char *)data)[i] == '\0') break;
    }
    printf("\n");
}

//places characters from n until the end of the src string,
//into the dest string
void break_memory(char* first, char* second, File file, int offset)
{
  int a = 0;
  int i = 0;
  for (i; i<offset; i++)
  {
    first[i] = file.mem[i];
  }
  i++;
  for (i; i<sizeof(file.mem); i++)
  {
    second[a] = file.mem[i];
    a++;
  }
}

char* combine_strings(File file, char* data, int abs_offset)
{
  char* final = malloc(sizeof(file.mem) + sizeof(data));
  char* first = malloc(sizeof(file.mem) - abs_offset + 1);
  char* second = malloc(sizeof(file.mem) - abs_offset);

  break_memory(first, second, file, abs_offset);

  /* printf("=====================================\n");
  printf("[DEBUG] Abs Offset = %d\n", abs_offset);
  printf("[DEBUG] First = "); print_hex(first);
  printf("[DEBUG] Data = "); print_hex(data);
  printf("[DEBUG] Second = "); print_hex(second); */

  strcat(final, first);
  strcat(final, data);
  strcat(final, second);

  /* printf("[DEBUG] Final = "); print_hex(final);
  printf("=====================================\n"); */
  return final;
}

// Return to provided methods //

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
  //char* true_mem = malloc(sizeof(unsigned char)*4);
  //read_file_from(file, true_mem, 4, SEEK_SET, 0L);
  bool seek_success = seek_file(file, start, offset);

  unsigned char *d = malloc(4*sizeof(unsigned char));
  unsigned long bytes_written = 0L;

  /* printf("=====================================\n");
  printf("[DEBUG] Stored mem value is: ");print_hex(file.mem);
  printf("[DEBUG] Actual mem value is: ");print_hex(true_mem);

  printf("[DEBUG] data value is: ");print_hex(data); */

  strcpy(d, combine_strings(file, data, ftell(file.fp)));

  /* printf("[DEBUG] d-value is: ");print_hex(d);
  printf("=====================================\n"); */

  fserror = NONE;
  if (! file.fp || ! seek_success) {fserror = WRITE_FAILED;}
  else if (d[0] == 0xFE && d[1] == 0xED && d[2] == 0xFA && (d[3] == 0xCE || d[3] == 0xCF)) 
  {
    // don't let 0xfeedface or 0xfeedfacf ever appear at the
    // beginning of the file!  IT CAN'T BE THIS EASY, CAN IT?
    fserror = ILLEGAL_MACHO;
  }
  else {
    bytes_written = fwrite(data, 1, num_bytes, file.fp);
    if (bytes_written < num_bytes) {fserror = WRITE_FAILED;}
    memcpy(file.mem, d, 4);
  }
  free(d);
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

