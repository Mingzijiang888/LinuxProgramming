#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/msg.h>

/*
 int msgget(key_t key, int msqflg); 
 msgget()创建或者获取一个消息队列
 msgget()成功返回消息队列 ID，失败返回-1
 msqflg： IPC_CREAT
 */

/*
 int msgsnd(int msqid, const void *msqp, size_t msqsz, int msqflg); 
 msgsnd()发送一条消息，消息结构为：
 struct msgbuf
 {
 long mtype; // 消息类型， 必须大于 0
 char mtext[1]; // 消息数据
 };
 msgsnd()成功返回 0， 失败返回-1
 msqsz： 指定 mtext 中有效数据的长度
 msqflg：一般设置为 0 可以设置 IPC_NOWAIT
 */

/*
 ssize_t msgrcv(int msqid, void *msgp, size_t msqsz, long msqtyp, int msqflg); 
 msgrcv()接收一条消息
 msgrcv()成功返回 mtext 中接收到的数据长度， 失败返回-1
 msqtyp： 指定接收的消息类型，类型可以为 0
 msqflg： 一般设置为 0 可以设置 IPC_NOWAIT
 */

/*
 int msgctl(int msqid, int cmd, struct msqid_ds *buf);
 msgctl()控制消息队列
 msgctl()成功返回 0，失败返回-1
 cmd： IPC_RMID
 */

typedef struct msgbuf
{
    long mtype; // 消息类型， 必须大于 0
    char mtext[128]; // 消息数据
} MsgData;


