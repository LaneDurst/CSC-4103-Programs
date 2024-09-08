#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
    unsigned long *p = malloc(sizeof(unsigned long));
    *p = 0xdeadbeefcafeba00;
    int i;

    for (i = 0; i<sizeof(unsigned long); i++)
    {
        printf("%02x ", ((unsigned char *)p)[i]);
    }

    printf("\n");
    return 0;
}