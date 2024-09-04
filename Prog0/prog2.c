#include <stdio.h>

typedef struct funcs
{
    int (*openit) (char *name, int prot);
    void (*closeit) (void);
} funcs;

int my_openit(char *name, int prot) 
{
    printf("Opened it.\n%s\n%d\n", name, prot);
    return 0;
}

void my_closeit(void) {printf("Closed it.\n");}

void list(funcs a)
{
    char *name = "Hello";
    a.openit(name, 12);
    a.closeit();
}

int main(int argc, char* argv[])
{
    funcs X = {.openit = my_openit, .closeit = my_closeit};
    list(X);
    return 0;
}