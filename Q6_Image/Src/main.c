#include "ImageConvert.h"


void test1()
{
	char sourcepath[100] = "./pic/1.jpg";
	char destpath[100] = "./pic/1.bmp";
	NK_JPG2BMP(sourcepath, destpath);
}

void test2()
{
	char sourcepath[100] = "./pic/rred.bmp";
	char destpath[100] = "./pic/rred.jpg";
	NK_BMP2JPG(sourcepath, destpath);
}

void test3()
{
	char sourcepath[100] = "./pic/2.bmp";
	char destpath[100] = "./pic/2.yuv";
	NK_BMP2YUV(sourcepath, destpath);
}

void test4()
{
	SIZEINFO size_info;
	size_info.height = 100;
	size_info.width = 100;
	char sourcepath[100] = "./pic/du.yuv";
	char destpath[100] = "./pic/du.bmp";
	NK_YUV2BMP(sourcepath, destpath, &size_info);
}

void test5()
{
	char sourcepath[100] = "./pic/j2y.jpg"; 
	char destpath[100] = "./pic/j2y.yuv";
	NK_JPG2YUV(sourcepath, destpath);
}

void test6()
{
	SIZEINFO size_info;
	size_info.height = 100;
	size_info.width = 100;
	char sourcepath[100] = "./pic/du.yuv";
	char destpath[100] = "./pic/du.jpg";
	NK_YUV2JPG(sourcepath, destpath, &size_info);
}

void test7()
{
    char sourcepath[100] = "./pic/rose.png";
	char destpath[100] = "./pic/rose.bmp";
	NK_PNG2BMP(sourcepath, destpath);
}

void test8()
{
    char sourcepath[100] = "./pic/rose.png";
	char destpath[100] = "./pic/rose.yuv";
	NK_PNG2YUV(sourcepath, destpath);
}

void test9()
{
    char sourcepath[100] = "./pic/rose.png";
	char destpath[100] = "./pic/rose.jpg";
	NK_PNG2JPG(sourcepath, destpath);
}

void test10()
{
    char sourcepath[100] = "./pic/rose.jpg";
	char destpath[100] = "./pic/rose.png";
	NK_JPG2PNG(sourcepath, destpath);
}

void test11()
{
    char sourcepath[100] = "./pic/rred.bmp";
	char destpath[100] = "./pic/rred.png";
	NK_BMP2PNG(sourcepath, destpath);
}

void test12()
{
    SIZEINFO size_info;
	size_info.height = 100;
	size_info.width = 100;
    char sourcepath[100] = "./pic/du.yuv";
	char destpath[100] = "./pic/du.png";
	NK_YUV2PNG(sourcepath, destpath, &size_info);
}

int main()
{
	// test1();
	test2();
	// test3();
	test4();
	// test5();
	// test6();
    // test7();
    // test8();
    // test9();
    test10();
    // test11();
    test12();
	return 0;
}
