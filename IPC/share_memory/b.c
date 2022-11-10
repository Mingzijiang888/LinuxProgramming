#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

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

//// /* Special key values.  */
//// #define IPC_PRIVATE	((__key_t) 0)	/* Private key.  */

int main()
{
    int shmid = shmget((key_t)1234, 128, IPC_CREAT | 0600);
    assert(shmid != -1);

    char* s = (char*)shmat(shmid, NULL, 0);
    assert(s != NULL);

    printf("shm_b s = %s\n", s);

    // 断开
    shmdt(s);
    // 消除shmid
    shmctl(shmid, IPC_RMID, NULL);

    exit(0);
}
