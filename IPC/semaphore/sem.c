#include <stdio.h>
#include "sem.h"

/*
 int semget(key_t key, int nsems, int semflg);
 semget()创建或者获取已存在的信号量
 semget()成功返回信号量的 ID， 失败返回-1
 key：两个进程使用相同的 key 值，就可以使用同一个信号量
 nsems：内核维护的是一个信号量集，在新建信号量时，其指定信号量集中信号量的个数
 semflg 可选： IPC_CREAT IPC_EXCL
 */

 
/*
 int semop(int semid, struct sembuf *sops, unsigned nsops);
 semop()对信号量进行改变，做 P 操作或者 V 操作
 semop()成功返回 0，失败返回-1
 struct sembuf
 {
    unsigned short sem_num; //指定信号量集中的信号量下标
    short sem_op; //其值为-1，代表 P 操作，其值为 1，代表 V 操作
    short sem_flg; //SEM_UNDO
 };
 */

/*
 int semctl(int semid, int semnum, int cmd, ...);
 semctl()控制信号量
 semctl()成功返回 0，失败返回-1
 cmd 选项： SETVAL IPC_RMID
 union semun
 {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *_buf;
 };
 */

//// /* Mode bits for `msgget', `semget', and `shmget'.  */
//// #define IPC_CREAT	01000		/* Create key if key does not exist. */
//// #define IPC_EXCL	02000		/* Fail if key exists.  */
//// #define IPC_NOWAIT	04000		/* Return error on wait.  */

//// /* Control commands for `msgctl', `semctl', and `shmctl'.  */
//// #define IPC_RMID	0		/* Remove identifier.  */
//// #define IPC_SET		1		/* Set `ipc_perm' options.  */
//// #define IPC_STAT	2		/* Get `ipc_perm' options.  */
//// #ifdef __USE_GNU
//// # define IPC_INFO	3		/* See ipcs.  */
//// #endif


static int semid = -1;

// 若为全新创建，则需要初始化；若已存在信号量，获得其id即可
void sem_init()
{
    // 判断是否全新创建
    // arg2:只创建1个信号量，arg3:希望全新创建且给其权限值
    semid = semget((key_t)1234, 1, IPC_CREAT | IPC_EXCL | 0600);
    // 失败代表可能已经存在了
    if (semid == -1)
    {
        // arg3:'0600'只做占位符作用，不起作用
        semid = semget((key_t)1234, 1, 0600);
        if (semid == -1)
        {
            perror("semget error.");
        }
    }
    else
    {
        // 全新创建，这里做初始化工作
        union semun semaphore;
        semaphore.val = 1;
        //man semctl: int semctl(int semid, int semnum, int cmd, ...)
        if (semctl(semid, 0, SETVAL, semaphore) == -1)
        {
            perror("semctl init error.");
        }
    }
}

void sem_p()
{
    struct sembuf buf;
    buf.sem_num = 0;
    buf.sem_op = -1;         // 减一相当于信号量p操作
    buf.sem_flg = SEM_UNDO;  // 异常时操作系统会帮忙归还信号量

    // arg3:操作的个数"1个"
    if (semop(semid, &buf, 1) == -1)
    {
        perror("p error.");
    }
}

void sem_v()
{
    struct sembuf buf;
    buf.sem_num = 0;
    buf.sem_op = 1;  // +1相当于信号量v操作
    buf.sem_flg = SEM_UNDO;

    // arg3:操作的个数"1个"
    if (semop(semid, &buf, 1) == -1)
    {
        perror("v error.");
    }
}

void sem_destroy()
{
    // arg2:靠semid标识要删除的，0只是做占位符；arg3:remove id
    if (semctl(semid, 0, IPC_RMID) == -1)
    {
        perror("destroy sem error.");
    }
}