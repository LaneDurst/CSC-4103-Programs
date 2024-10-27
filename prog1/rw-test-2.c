// Compile:
//
// gcc -o rw-test-2 rw-test-2.c fileio.c
// 
// Run this test AFTER rw-test-1.  DO NOT delete "badmacho.dat" before
// running this test.
// 

#include <stdio.h>
#include <unistd.h>
#include "fileio.h"

int main(int argc, char *argv[]) {
  
  File f;

  printf("\nAbout to open file \"badmacho.dat\"...\n");
  f=open_file("badmacho.dat");
  fs_print_error();

  printf("\nAbout to write bytes 0xED, 0xFA, 0xCE (should fail with ILLEGAL_MACHO)...\n");
  write_file_at(f, "\xED\xFA\xCE", 3, END_OF_FILE, 0L);
  fs_print_error();

  printf("\nAbout to close file \"badmacho.dat\"...\n");
  close_file(f);
  fs_print_error();

  return 0;
}
  

  

