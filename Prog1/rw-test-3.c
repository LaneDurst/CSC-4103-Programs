// Compile:
//
// gcc -o rw-test-3 rw-test-3.c fileio.c
// 
// This is a standalone test.
// 

#include <stdio.h>
#include <unistd.h>
#include "fileio.h"

int main(int argc, char *argv[]) {

  File f;
  
  unlink("another.dat");

  printf("\nAbout to open file \"another.dat\"...\n");
  f=open_file("another.dat");
  fs_print_error();

  printf("\nAbout to write HELLOTHERE...\n");
  write_file_at(f, "HELLOTHERE", strlen("HELLOTHERE"), BEGINNING_OF_FILE, 0L);
  fs_print_error();

  printf("\nAbout to write one byte (0xCF)...\n");
  write_file_at(f, "\xCF", 1, END_OF_FILE, -7L);
  fs_print_error();

  printf("\nAbout to write one byte (0xCF)...\n");
  write_file_at(f, "\xFA", 1, END_OF_FILE, -8L);
  fs_print_error();

  printf("\nAbout to write one byte (0xED)...\n");
  write_file_at(f, "\xED", 1, END_OF_FILE, -9L);
  fs_print_error();

  printf("\nAbout to write one byte (0xFE) (should fail with ILLEGAL_MACHO)...\n");
  write_file_at(f, "\xFE", 1, END_OF_FILE, -10L);  
  fs_print_error();
  
  printf("\nAbout to close file \"another.dat\"...\n");
  f=open_file("another.dat");
  fs_print_error();
  
  return 0;
}
  

  

