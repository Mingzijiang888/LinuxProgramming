#include <sys/sem.h>
#include <unistd.h>

union semun
{
    // 需要自己定义，见手册man semctl
    int val;
};

void sem_init();
void sem_p();
void sem_v();
void sem_destroy();
