#include "../Include/List.h"

/**
 * @brief 创建一条新链表，并初始化其属性
 * 
 * @param pList ，需要初始化的链表，新建头尾哨兵相互指向
 * @param data_size ，节点的数据大小
 * @return int ，成功返回0，失败返回-1
 */
int NK_LIST_InitAList(LIST_S** pList, int data_size)
{
	int ret = 0;
    if (pList == NULL)
    {
        printf("[Line:%d] list not exist\n", __LINE__);
		return -1;
    }

    *pList = (LIST_S*)malloc(sizeof(LIST_S));
    if (*pList == NULL)
	{
		printf("[Line:%d] malloc failed\n", __LINE__);
		return -1;
	}
	memset(*pList, 0, sizeof(LIST_S));

	ret = pthread_rwlock_init(&((*pList)->rwlock), NULL);
	if (ret != 0)	// rwlock init error
	{
		printf("[Line:%d] rwlock init failed\n", __LINE__);
		free(pList);
		pList = NULL;
		return -1;
	}

    (*pList)->header = (NODE_S*)malloc(sizeof(NODE_S));
	(*pList)->tailer = (NODE_S*)malloc(sizeof(NODE_S));
	if ((*pList)->header == NULL || (*pList)->tailer == NULL)
	{
		printf("[Line:%d] malloc failed\n", __LINE__);
		free(pList);
		pList = NULL;
		return -1;
	}
	memset((*pList)->header, 0, sizeof(NODE_S));
	memset((*pList)->tailer, 0, sizeof(NODE_S));

	(*pList)->header->prev = NULL;
	(*pList)->header->next = (*pList)->tailer;
	(*pList)->tailer->next = NULL;
	(*pList)->tailer->prev = (*pList)->header;
    (*pList)->data_size = data_size;
	(*pList)->length = 0;

	(*pList)->NK_COMPARE_FN = NULL;
	(*pList)->NK_DESTROY_FN = NULL;
	(*pList)->NK_TRAVERSE_FN = NULL;

	return 0;
}

/**
 * @brief 新建一个节点指针，并申请节点空间
 * 
 * @param void* pData ，节点的数据指针
 * @param datasize ，节点的数据大小
 * @return NODE_S* ，成功返回节点指针，失败返回NULL
 */
static NODE_S* NK_LIST_CreateNewNode(void* pData, int datasize)
{
	NODE_S* pNode = (NODE_S*)malloc(sizeof(NODE_S));
	if (pNode == NULL)
	{
		printf("[Line:%d] malloc failed\n", __LINE__);
		return NULL;
	}
	memset(pNode, 0, sizeof(NODE_S));

	pNode->pData = (void*)malloc(datasize);
	if (pNode->pData == NULL)
	{
        free(pNode);
		pNode = NULL;
		printf("[Line:%d] malloc failed\n", __LINE__);
		return NULL;
	}
	memset(pNode->pData, 0, sizeof(NODE_S));

	memcpy(pNode->pData, pData, datasize);		// deep copy
	
	return pNode;
}


/**
 * @brief 在传入的某节点（当前节点）前插入一个节点
 * 
 * @param thisnode ，当前节点
 * @param pData ，节点的数据指针
 * @param datasize ，节点的数据大小
 * @return int ，成功返回0，失败返回-1
 */
static int NK_NODE_InsertAsPrev(NODE_S* thisnode, void* pData, int datasize)
{
	NODE_S* newnode = NK_LIST_CreateNewNode(pData, datasize);
	if (newnode == NULL)
	{
		return -1;
	}

	newnode->next = thisnode;
	newnode->prev = thisnode->prev;
	thisnode->prev->next = newnode;
	thisnode->prev = newnode;
	return 0;
}

/**
 * @brief 在传入的某节点（当前节点）后插入一个节点
 * 
 * @param thisnode ，当前节点
 * @param pData ，节点的数据指针
 * @param datasize ，节点的数据大小
 * @return int ，成功返回0，失败返回-1
 */
static int NK_NODE_InsertAsNext(NODE_S* thisnode, void* pData, int datasize)
{
	NODE_S* newnode = NK_LIST_CreateNewNode(pData, datasize);
	if (newnode == NULL)
	{
		return -1;
	}

	newnode->prev = thisnode;
	newnode->next = thisnode->next;
	thisnode->next->prev = newnode;
	thisnode->next = newnode;
	return 0; 
}

/**
 * @brief 插入一个节点作为链表的尾节点
 * 
 * @param L ，当前链表
 * @param pData ，节点的数据指针
 * @return int ，成功返回0，失败返回-1
 */
int NK_LIST_InsertAsLast(LIST_S* L, void* pData)
{
	int ret = 0;
	if (L == NULL)
	{
		printf("[Line:%d] List not exists\n", __LINE__);
		return -1;
	}
	pthread_rwlock_wrlock(&(L->rwlock));
	if (pData == NULL)
	{
		printf("[Line:%d] Data not exists\n", __LINE__);
		pthread_rwlock_unlock(&(L->rwlock));
		return -1;
	}
	
	ret = NK_NODE_InsertAsPrev(L->tailer, pData, L->data_size);
	// NODE_S* newnode = NK_LIST_CreateNewNode(pData, datasize);
	// (*L)->tailer->prev->next = newnode;
	// (*L)->tailer->prev = newnode;
	if (ret == -1)
	{
		printf("[Line:%d] Insert Error\n", __LINE__);
		pthread_rwlock_unlock(&(L->rwlock));
		return -1;
	}

	L->length++;
	pthread_rwlock_unlock(&(L->rwlock));
	return 0;
}

/**
 * @brief 插入一个节点作为链表的头节点
 * 
 * @param L ，当前链表
 * @param pData ，节点的数据指针
 * @return int ，成功返回0，失败返回-1
 */
int NK_LIST_InsertAsFirst(LIST_S* L, void* pData)
{
	int ret = 0;
	if (L == NULL)
	{
		printf("[Line:%d] List not exists\n", __LINE__);
		return -1;
	}

	pthread_rwlock_wrlock(&(L->rwlock));
	if (pData == NULL)
	{
		printf("[Line:%d] Data not exists\n", __LINE__);
		pthread_rwlock_unlock(&(L->rwlock));
		return -1;
	}

	ret = NK_NODE_InsertAsNext(L->header, pData, L->data_size);
	if (ret == -1)
	{
		printf("[Line:%d] Insert Error\n", __LINE__);
		pthread_rwlock_unlock(&(L->rwlock));
		return -1;
	}
	
	L->length++;
	pthread_rwlock_unlock(&(L->rwlock));
	return 0;
}

/**
 * @brief 根据链表的回调函数NK_COMPARE_FN，插入一个新节点作为当前链表中第一个符合规则的节点的前驱，
 * 		  若当前链表没有符合规则的节点，默认插入到尾节点处
 * 
 * @param L ，当前链表
 * @param pData ，节点的数据指针
 * @return int ，成功返回0，失败返回-1
 */
int NK_LIST_InsertBySequence(LIST_S* L, void* pData)
{
	int ret = 0;
	if (L == NULL)
	{
		printf("[Line:%d] List not exists\n", __LINE__);
		return -1;
	}

	pthread_rwlock_wrlock(&(L->rwlock));
	// if (ret != 0)
	// {
	// 	printf("[Line:%d] Acquire rwlock failed\n", __LINE__);
	// 	return -1;
	// }
	if (L->NK_COMPARE_FN == NULL)
	{
		printf("[Line:%d] NK_COMPARE_FN not defined\n", __LINE__);
		pthread_rwlock_unlock(&(L->rwlock));
		return -1;
	}
	if (pData == NULL)
	{
		printf("[Line:%d] Data not exists\n", __LINE__);
		pthread_rwlock_unlock(&(L->rwlock));
		return -1;
	}

	// while list is empty
	if (L->length == 0)
	{
		//NK_LIST_InsertAsFirst(L, pData);		// previously I write it "*pData" making the first insert node rubbish_value
		ret = NK_NODE_InsertAsNext(L->header, pData, L->data_size);
		if (ret == -1)
		{
			printf("[Line:%d] Insert Error\n", __LINE__);
			pthread_rwlock_unlock(&(L->rwlock));
			return -1;
		}
		L->length++;
		
		pthread_rwlock_unlock(&(L->rwlock));
		if (ret != 0)
		{
			printf("[Line:%d] Unlock rwlock failed\n", __LINE__);
			return -1;
		}
		return 0;
	}

	NODE_S* curr_node = L->header->next;

	
	while (curr_node->next != NULL && curr_node->next->next != NULL)
	{
		if ((*(L->NK_COMPARE_FN))(pData, curr_node->pData) == 1)
		{
			ret = NK_NODE_InsertAsPrev(curr_node, pData, L->data_size);
			if (ret != 0)
			{
				printf("[Line:%d] Unlock rwlock failed\n", __LINE__);
				return -1;
			}
			L->length++;
			pthread_rwlock_unlock(&(L->rwlock));
			return 0;

		}
		curr_node = curr_node->next;
	}
	if ((*(L->NK_COMPARE_FN))(pData, curr_node->pData) == 1)
	{
		ret = NK_NODE_InsertAsPrev(curr_node, pData, L->data_size);		// return immediately once successfully
		if (ret == -1)
		{
			printf("[Line:%d] fail to insert\n", __LINE__);
			return -1;
		}
		L->length++;
		pthread_rwlock_unlock(&(L->rwlock));
		return 0;
	}
	else
	{
		ret = NK_NODE_InsertAsNext(curr_node, pData, L->data_size);		// return immediately once successfully
		if (ret == -1)
		{
			printf("[Line:%d] fail to insert\n", __LINE__);
			return -1;
		}
		L->length++;
		pthread_rwlock_unlock(&(L->rwlock));
		return 0;
	}
	
}

/**
 * @brief 删除当前节点
 * 
 * @param deletenode 
 * @return void* 返回当前节点的数据指针
 */
static void* NK_NODE_Remove(NODE_S* deletenode)
{
	void* pRet = deletenode->pData;
	(deletenode->prev)->next = deletenode->next; 
	(deletenode->next)->prev = deletenode->prev;
	free(deletenode);
	deletenode = NULL;
	return pRet;
}

/**
 * @brief 删除链表的尾节点
 * 
 * @param L ，当前链表
 * @return void* ，成功返回被删除节点的数据指针，失败返回NULL
 */
void* NK_LIST_RemoveFromTail(LIST_S* L)
{
	void* pRet = NULL;
	if (L == NULL)
	{
		printf("[Line:%d] List not exists\n", __LINE__);
		return NULL;
	}
	pthread_rwlock_wrlock(&(L->rwlock));
    if (L->length == 0)
    {
        printf("[Line:%d] List have not node now\n", __LINE__);
		pthread_rwlock_unlock(&(L->rwlock));
		return NULL;
    }

	NODE_S* pNode = L->tailer->prev;
	pRet = pNode->pData;

	L->tailer->prev = pNode->prev;
	pNode->prev->next = L->tailer;
	free(pNode);
	pNode = NULL;

	L->length--;
	pthread_rwlock_unlock(&(L->rwlock));
	return pRet;
}

/**
 * @brief 删除链表的头节点
 * 
 * @param L ，当前链表
 * @return void* ，成功返回被删除节点的数据指针，失败返回NULL
 */
void* NK_LIST_RemoveFromHead(LIST_S* L)
{
	void* pRet = NULL;
	if (L == NULL)
	{
		printf("[Line:%d] List not exists\n", __LINE__);
		return NULL;
	}
	pthread_rwlock_wrlock(&(L->rwlock));
    if (L->length == 0)
    {
        printf("[Line:%d] List have not node now\n", __LINE__);
		pthread_rwlock_unlock(&(L->rwlock));
		return NULL;
    }
	
	NODE_S* pNode = L->header->next;
	pRet = pNode->pData;

	L->header->next = pNode->next;
	pNode->next->prev = L->header;
	free(pNode);
	pNode = NULL;

	L->length--;
	pthread_rwlock_unlock(&(L->rwlock));
	return pRet;
}

/**
 * @brief 根据链表的回调函数NK_DESTROY_FN，删除链表中第一个符合规则的节点
 * 
 * @param L ，当前链表
 * @param pData ，指定数据的指针
 * @param equal ，equal回调函数，成功返回0，失败返回-1
 * @return void* ，成功返回被删除节点的数据指针，失败返回NULL
 */
void* NK_LIST_RemoveByData(LIST_S* L, void* pData)
{
	void* pRet = NULL;
    if (L == NULL)
	{
		printf("[Line:%d] List not exists\n", __LINE__);
		return NULL;
	}
	pthread_rwlock_wrlock(&(L->rwlock));
	if (L->NK_DESTROY_FN == NULL)
	{
		printf("[Line:%d] NK_DESTROY_FN not defined\n", __LINE__);
		pthread_rwlock_unlock(&(L->rwlock));
		return NULL;
	}
    if (L->length == 0)
    {
        printf("[Line:%d] List have not node now\n", __LINE__);
		pthread_rwlock_unlock(&(L->rwlock));
		return NULL;
    }
	if (pData == NULL)
	{
		printf("[Line:%d] Data not exists\n", __LINE__);
		pthread_rwlock_unlock(&(L->rwlock));
		return NULL;
	}

    NODE_S* curr_node = L->header->next;

    while (curr_node->next != NULL && curr_node->next->next != NULL)
    {
        if ((*(L->NK_DESTROY_FN))(pData, curr_node->pData) == 1)
        {
            pRet = NK_NODE_Remove(curr_node);
        }
		curr_node = curr_node->next;
    }
    if ((*(L->NK_DESTROY_FN))(pData, curr_node->pData) == 1)
	{
		pRet = NK_NODE_Remove(curr_node);
	}

	pthread_rwlock_unlock(&(L->rwlock));
	L->length--;
    return pRet;
}

/**
 * @brief 清空链表，并清除其中各节点的占用空间
 * 
 * @param L ，当前链表
 * @return int ，成功返回被删除链表的长度，失败（链表为空）返回0
 */
int NK_LIST_MakeEmpty(LIST_S** L)
{
	if ((*L) == NULL)
	{
        printf("[Line:%d] List not exists\n", __LINE__);
		return 0;
	}

	int oldlength = (*L)->length;
	while ((*L)->length > 0)
	{
		NK_LIST_RemoveFromHead((*L));      // (*L)->length-- inside
	}

	free((*L)->header);
	(*L)->header = NULL;
	free((*L)->tailer);
	(*L)->tailer = NULL;

	pthread_rwlock_destroy(&((*L)->rwlock));

	free(*L);
	(*L) = NULL;
    return oldlength;
}

/**
 * @brief 根据链表的回调函数NK_TRAVERSE_FN，进行节点的遍历并输出其数据
 * 
 * @param L ，当前链表
 */
void NK_LIST_Traverse(LIST_S* L)
{
	if (L == NULL)
	{
        printf("[Line:%d] List not exists\n", __LINE__);
		
		return;
	}
	pthread_rwlock_rdlock(&(L->rwlock));
	if (L->NK_TRAVERSE_FN == 0)
	{
		printf("[Line:%d] NK_TRAVERSE_FN not defined\n", __LINE__);
		pthread_rwlock_unlock(&(L->rwlock));
		return;
	}
	if (L->length == 0)
    {
        printf("NULL\n");
		pthread_rwlock_unlock(&(L->rwlock));
		return;
    }
	
	NODE_S* curr_node = L->header->next;

	while (curr_node->next != NULL && curr_node->next->next != NULL)
	{
		(*(L->NK_TRAVERSE_FN))(curr_node->pData);
		curr_node = curr_node->next;
	}
	(*(L->NK_TRAVERSE_FN))(curr_node->pData);
	printf("NULL\n");
	
	pthread_rwlock_unlock(&(L->rwlock));
}


