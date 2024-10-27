// Compile:
//
// gcc -o rw-test-4 rw-test-4.c fileio.c
// 
// This is a standalone test.
//

#include <stdio.h>
#include <unistd.h>
#include "fileio.h"

int main(int argc, char *argv[]) {
  
  File f1, f2;
  
  unlink("one.dat");
  unlink("two.dat");

  printf("\nAbout to open file \"one.dat\"...\n");
  f1=open_file("one.dat");
  fs_print_error();

  printf("\nAbout to write bytes 0xED, 0xFA, 0xCE to \"one.dat\"...\n");  
  write_file_at(f1, "\xED\xFA\xCE", 3, BEGINNING_OF_FILE, 1L);
  fs_print_error();

  printf("\nAbout to open file \"two.dat\"...\n");
  f2=open_file("two.dat");
  fs_print_error();

  printf("\nAbout to write AB to \"two.dat\"...\n");  
  write_file_at(f2, "AB", 2, BEGINNING_OF_FILE, 0L);
  fs_print_error();

  printf("\nAbout to write byte (0xFE) to \"one.dat\" (should fail with ILLEGAL_MACHO)...\n");  
  write_file_at(f1, "\xFE", 1, BEGINNING_OF_FILE, 0L);
  fs_print_error();

  printf("\nAbout to close file \"one.dat\"...\n");
  close_file(f1);
  fs_print_error();

  printf("\nAbout to close file \"two.dat\"...\n");
  close_file(f2);
  fs_print_error();
  return 0;
}
  

  

