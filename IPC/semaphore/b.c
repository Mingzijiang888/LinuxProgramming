#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sem.h"

int main()
{
    int i = 0;
    sem_init();
    for (i = 0; i < 5; i++)
    {
        sem_p();

        printf("B");
        fflush(stdout);
        int n = rand() % 3;
        sleep(n);
        printf("B");
        fflush(stdout);

        sem_v();

        n = rand() % 3;
        sleep(n);
    }
}


