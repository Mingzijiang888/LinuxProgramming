#include <stdio.h>
#include "max.h" 	////无需写出具体路径，在gcc编译时-I指定头文件搜寻路径

int add(int x, int y);

int main()
{
	int a = 2;
	int b =3;

	printf("add = %d\n", add(a,b));
	printf("max = %d\n", MAX(a,b));
	return 0;
}
