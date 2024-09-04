#include <stdio.h>

#define TOTAL_ELEMENTS ((sizeof(array) / sizeof(array[0])))
int array[] = {1, 3, 15, 19, 0, 43, 12};

int main(int argc, char *argv[])
{
    int size = TOTAL_ELEMENTS;
    int i;

    for (i = -1; i <= size-2; i++)
    {
        printf("Element %d has value %d\n", i+1, array[i+1]);
    }

    return 0;
}