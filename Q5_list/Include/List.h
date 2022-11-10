#ifndef _LIST_H_
#define _LIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef struct NODE_S  NODE_S;
typedef struct LIST_S  LIST_S;

struct NODE_S
{
	void* pData;
	NODE_S* prev;
	NODE_S* next;
} ;

struct LIST_S
{
	NODE_S* header;
	NODE_S* tailer;
	int length;			// Scale of the list
    int data_size;
	// int data_type;
	int (*NK_DESTROY_FN)(void* pReferData, void* pCurrData);
	int (*NK_COMPARE_FN)(void* pReferData, void* pCurrData);
	void (*NK_TRAVERSE_FN)(void* pData);
	pthread_rwlock_t rwlock;
} ;

int NK_LIST_InitAList(LIST_S** pList, int data_size);
int NK_LIST_InsertAsLast(LIST_S* L, void* pData);
int NK_LIST_InsertAsFirst(LIST_S* L, void* pData);
int NK_LIST_InsertBySequence(LIST_S* L, void* pData);
void* NK_LIST_RemoveFromTail(LIST_S* L);
void* NK_LIST_RemoveFromHead(LIST_S* L);
void* NK_LIST_RemoveByData(LIST_S* L, void* pData);
int NK_LIST_MakeEmpty(LIST_S** L);
void NK_LIST_Traverse(LIST_S* L);

#endif
