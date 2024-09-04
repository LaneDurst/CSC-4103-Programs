#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char* argv[])
{
    /* Debugg stuff
    printf("argc is: %d\n", argc);
    for (int i = 0; i < argc; i++)
    {
        printf("argv[%d]: %s\n", i, argv[i]);
    }
    */

    if (argc)
    {
        const char* filename = NULL;
        filename = argv[1];

        //printf("Filename: %s\n", filename);

        FILE *fp = fopen(filename, "r");
        if (fp == NULL) {printf("File could not be opened.\nExiting Program\n"); return 1;}
        //else printf("Opened file succesfully.\n");

        fseek(fp, 0L, SEEK_END); //move fp to the end of the file. 0L is there to tell it not to move from the SEEK_END
        long int file_size = ftell(fp); //tells location of file pointer (fp) in the file as an long integer
        fseek(fp, 0L, SEEK_SET); //move fp to the begging of the file.

        //printf("File is %ld bytes\n", file_size);

        //we do this because we have to malloc things given at runtime (by the user)
        //but not stuff we get at startup of the program
        char* file_contents = malloc(file_size+1);
        if (file_contents == NULL) {printf("Malloc failed.\nExiting program.\n"); return 1;}
        fread(file_contents, sizeof(char), file_size, fp);
        //printf("Read file Contents.\n");

        fclose(fp);

        strcat(file_contents, "\0");

        char *tmp = strtok(file_contents, " \t\n,");

        while (tmp != NULL)
        {
            printf("%s\n", tmp);
            tmp = strtok(NULL, " \t\n,");
        }

        free(file_contents);

        return 0;

    }
    else
    {
        printf("No Filename was given for the program to open.\nExiting Program.");
        return 1;
    }
}