#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 1024

int cmp(const void* left, const void* right)
{   
    return strcasecmp(*(const char **) left, *(const char **) right);
}

int main(int argc, char* argv[])
{
    int retry = 1;
    char** strings = NULL;
    int n;

    while (retry)
    {
        printf("Please input an integer value: ");
        fflush(stdout);
        int success = scanf("%d", &n); //boolean value to check if scanf actually found a number
        if (success)
        {
            getchar(); //removing the \n stored in the stream from scanf

            strings = (char**)malloc(n * sizeof(char*)); //malloc for the array itself
            if (strings == NULL) {fprintf(stderr, "Malloc failed\n"); return 1;} //error handling

            for(int i = 0; i < n; i++)
            {
                char tmp[BUFSIZE+1]; //temporary storage until we are done with input string
                printf("Please input string #%d: ", i+1);
                fgets(tmp, BUFSIZE-1, stdin);
                tmp[strcspn(tmp,"\n")] = '\0'; //replaces any \n with \0

                strings[i] = (char*)malloc(sizeof(tmp)); //malloc for each item in the array
                if (strings[i] == NULL) {fprintf(stderr, "Malloc failed\n"); return 1;} //error handling

                strcpy(strings[i], tmp);
            }
            retry = 0;
        }
        else
        {
            getchar(); //removing the \n stored in the stream from scanf
            printf("You did not enter an integer\n");
        }
    }

    qsort(strings, n, sizeof(strings[0]), cmp);
    printf("===============\n");
    printf("Sorted Strings:\n");
    for(int i = 0; i < n; i++)
    {
        printf("[#%d] %s\n", i+1, strings[i]);
    }
    printf("===============\n");

    for(int i = 0; i < n; i++)
    {
        free(strings[i]);
    }
    free(strings);
    return 0;
}