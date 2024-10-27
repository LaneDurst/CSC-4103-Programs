// Compile:
//
// gcc -o rw-test-0 rw-test-0.c fileio.c
// 
// This is a standalone test.
// 

#include <stdio.h>
#include <unistd.h>
#include "fileio.h"

#define DATA1 "\xFE\xED\xFA\xCE"
#define DATA2 "\xFE\xED\xFA\xCF"
#define DATA3 "HELLO THERE!"

int main(int argc, char *argv[]) {
  
  File f;

  unlink("macho.dat");
  
  printf("\nAbout to open file \"macho.dat\"...\n");
  f=open_file("macho.dat");
  fs_print_error();

  printf("\nAbout to write DATA1 (should fail  with ILLEGAL_MACHO)...\n");
  write_file_at(f, DATA1, strlen(DATA1), BEGINNING_OF_FILE, 0L);
  fs_print_error();

  printf("\nAbout to write DATA2 (should fail with ILLEGAL_MACHO)... \n");
  write_file_at(f, DATA2, strlen(DATA2), BEGINNING_OF_FILE, 0L);
  fs_print_error();

  printf("\nAbout to write DATA3...\n");
  write_file_at(f, DATA3, strlen(DATA3), BEGINNING_OF_FILE, 0L);
  fs_print_error();

  printf("\nAbout to close file \"macho.dat\"...\n");
  close_file(f);
  fs_print_error();
  return 0;
}
  

  

