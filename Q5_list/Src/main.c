#include "../Include/List.h"
#include <unistd.h>

// typedef struct NODE_S  NODE_S;
// typedef struct LIST_S  LIST_S;

#define  my_print2(format,...)    printf("\033[0;31m[%s:%d]%s\033[m\n", __FUNCTION__, __LINE__, format, ##__VA_ARGS__)

void TRAVERSE_INT(void* pData)
{
	printf("%d->", *(int*)pData);
}
void TRAVERSE_STR(void* pData)
{
	printf("%s->", (char*)pData);
}

int COMPARE_INT(void* pReferData, void* pCurrData)
{
	return *(int*)pCurrData >= *(int*)pReferData ? 1 : 0;
}
int COMPARE_STR(void* pReferData, void* pCurrData)
{
	return (strcmp(pCurrData, pReferData) >= 0) ? 1 : 0;
	
}

int DESTROY_INT(void* pReferData, void* pCurrData)
{
	return *(int*)pCurrData == *(int*)pReferData ? 1 : 0;
}
int DESTROY_STR(void* pReferData, void* pCurrData)
{
	return strcmp((char*)pCurrData, (char*)pReferData) == 0 ? 1 : 0;
}

void test1()
{
	int x = 0;
	LIST_S* list1 = NULL;
	
    NK_LIST_InitAList(&list1, sizeof(int));
	list1->NK_COMPARE_FN = COMPARE_INT;
	list1->NK_DESTROY_FN = DESTROY_INT;
	list1->NK_TRAVERSE_FN = TRAVERSE_INT;
	
	x = 43523434;
	NK_LIST_InsertBySequence(list1, &x);
	x = 33654547;
	NK_LIST_InsertBySequence(list1, &x);
	x = 56789332;
	NK_LIST_InsertBySequence(list1, &x);
	x = 98765;
	NK_LIST_InsertBySequence(list1, &x);
	x = 293847;
	NK_LIST_InsertBySequence(list1, &x);

	x = 98765;
	NK_LIST_RemoveByData(list1, &x);
	printf("length = %d\n", list1->length);

	NK_LIST_Traverse(list1);
	
}

void test2()
{
	int x = 0;
	// test  -- "empty"
	LIST_S* list2 = NULL;
	NK_LIST_InitAList(&list2, sizeof(int));
	list2->NK_COMPARE_FN = COMPARE_INT;
	list2->NK_DESTROY_FN = DESTROY_INT;
	list2->NK_TRAVERSE_FN = TRAVERSE_INT;

    NK_LIST_RemoveFromTail(list2);
	x = 43523434;
	NK_LIST_InsertBySequence(list2, &x);
	x = 33654547;
	NK_LIST_InsertBySequence(list2, &x);

    printf("%d\n", NK_LIST_MakeEmpty(&list2));

	NK_LIST_Traverse(list2);
}

void test3()
{
	char* x = "\0";
	x = (char*)malloc(sizeof(char) * 100);

	LIST_S* list1 = NULL;
    NK_LIST_InitAList(&list1, 100);
	list1->NK_COMPARE_FN = COMPARE_STR;
	list1->NK_TRAVERSE_FN = TRAVERSE_STR;
	list1->NK_DESTROY_FN = DESTROY_STR;

	x = "fai";
	NK_LIST_InsertBySequence(list1, x);
	x = "12431";
	NK_LIST_InsertBySequence(list1, x);
	x = "3r82";
	NK_LIST_InsertBySequence(list1, x);
	x = "ry8h";
	NK_LIST_InsertBySequence(list1, x);
	x = "yfhc";
	NK_LIST_InsertBySequence(list1, x);

	x = "3r82";
	NK_LIST_RemoveByData(list1, x);

	NK_LIST_Traverse(list1);
}



// test4, test5像如下这样定义时，编译会报错称函数指针与pthread_create函数第三个参数不匹配
// 所以对线程函数应该传void*指针，在函数中再进行类型强制转化，如LIST_S* tmp_list = (LIST_S*)list1;
// void* test4(LIST_S* list1)
// void* test5(LIST_S* list1)

void* test4(void* list1)
{
	int x = 0;
	
	LIST_S* tmp_list = (LIST_S*)list1;

	x = 4;
	NK_LIST_InsertBySequence(tmp_list, &x);
	NK_LIST_Traverse(tmp_list);
	sleep(rand() % 3);

	x = 2;
	NK_LIST_InsertBySequence(tmp_list, &x);
	NK_LIST_Traverse(tmp_list);
	sleep(rand() % 3);

	x = 3;
	NK_LIST_InsertBySequence(tmp_list, &x);
	NK_LIST_Traverse(tmp_list);
	sleep(rand() % 3);

	x = 5;
	NK_LIST_InsertBySequence(tmp_list, &x);
	NK_LIST_Traverse(tmp_list);
	sleep(rand() % 3);

	x = 1;
	NK_LIST_InsertBySequence(tmp_list, &x);
	NK_LIST_Traverse(tmp_list);
	sleep(rand() % 3);

	x = 4;
	NK_LIST_RemoveByData(tmp_list, &x);
	NK_LIST_Traverse(tmp_list);

	printf("[Line:%d] length = %d\n", __LINE__, tmp_list->length);

	return NULL;
}

void* test5(void* list1)
{
	int x = 0;
	
	LIST_S* tmp_list = (LIST_S*)list1;

	x = 7;
	NK_LIST_InsertBySequence(tmp_list, &x);
	NK_LIST_Traverse(tmp_list);
	sleep(rand() % 3);

	x = 10;
	NK_LIST_InsertBySequence(tmp_list, &x);
	NK_LIST_Traverse(tmp_list);
	sleep(rand() % 3);

	x = 9;
	NK_LIST_InsertBySequence(tmp_list, &x);
	NK_LIST_Traverse(tmp_list);
	sleep(rand() % 3);

	x = 6;
	NK_LIST_InsertBySequence(tmp_list, &x);
	NK_LIST_Traverse(tmp_list);
	sleep(rand() % 3);

	x = 8;
	NK_LIST_InsertBySequence(tmp_list, &x);
	NK_LIST_Traverse(tmp_list);
	sleep(rand() % 3);

	x = 6;
	NK_LIST_RemoveByData(tmp_list, &x);
	NK_LIST_Traverse(tmp_list);

	printf("[Line:%d] length = %d\n", __LINE__, tmp_list->length);

	return NULL;
}

// multi-pthread test
int test6()
{
	LIST_S* list1 = NULL;
	
    NK_LIST_InitAList(&list1,  sizeof(int));
	list1->NK_COMPARE_FN = COMPARE_INT;
	list1->NK_DESTROY_FN = DESTROY_INT;
	list1->NK_TRAVERSE_FN = TRAVERSE_INT;
	
	pthread_t thread1;
	pthread_t thread2;

	if(pthread_create(&thread1, NULL, test4, list1))
    {
        printf("thread 1 create failed\n");
        return -1;
    }

    if(pthread_create(&thread2, NULL, test5, list1))
    {
        printf("thread 2 create failed\n");
        return -1;
    }

	pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

	return 0;	
}


int main()
{
	int ret = 0;

	// test1();
	// putchar('\n');
	// test2();
	// putchar('\n');
	// test3();
	// putchar('\n');

	ret = test6();
	printf("\nreturn code = %d\n", ret);

	return 0;
}
