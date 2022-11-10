#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
/*
 int shmget(key_t key, size_t size, int shmflg);
 shmget()用于创建或者获取共享内存
 shmget()成功返回共享内存的 ID， 失败返回-1
 key： 不同的进程使用相同的 key 值可以获取到同一个共享内存
 size： 创建共享内存时，指定要申请的共享内存空间大小
 shmflg: IPC_CREAT IPC_EXCL
*/

/*
 void* shmat(int shmid, const void *shmaddr, int shmflg);
 shmat()将申请的共享内存的物理内存映射到当前进程的虚拟地址空间上
 shmat()成功返回返回共享内存的首地址，失败返回 NULL
 shmaddr：一般给 NULL，由系统自动选择映射的虚拟地址空间
 shmflg： 一般给 0， 可以给 SHM_RDONLY 为只读模式，其他的为读写
*/
 
/*
 int shmdt(const void *shmaddr);
 shmdt()断开当前进程的 shmaddr 指向的共享内存映射
 shmdt()成功返回 0， 失败返回-1
*/
 
/*
 int shmctl(int shmid, int cmd, struct shmid_ds *buf);
 shmctl()控制共享内存
 shmctl()成功返回 0，失败返回-1
 cmd： IPC_RMID
*/

int main()
{
    int shmid = shmget((key_t)1234, 128, IPC_CREAT | 0600);
    assert(shmid != -1);

    char* s = (char*)shmat(shmid, NULL, 0);
    assert(s != NULL);

    strcpy(s, "hello");

    shmdt(s);
    exit(0);
}
