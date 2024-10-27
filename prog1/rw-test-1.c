// Compile:
//
// gcc -o rw-test-1 rw-test-1.c fileio.c
// 
// Run this test BEFORE rw-test-2.
// 

#include <stdio.h>
#include <unistd.h>
#include "fileio.h"

int main(int argc, char *argv[]) {
  
  File f;
  
  unlink("badmacho.dat");

  printf("\nAbout to open file \"badmacho.dat\"...\n");
  f=open_file("badmacho.dat");
  fs_print_error();

  printf("\nAbout to write one byte (0xFE)...\n");
  write_file_at(f, "\xFE", 1, BEGINNING_OF_FILE, 0L);
  fs_print_error();

  printf("\nAbout to close file \"badmacho.dat\"...\n");
  close_file(f);
  fs_print_error();
  
  return 0;
}
  

  

