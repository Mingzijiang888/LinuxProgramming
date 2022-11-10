#ifndef _IMAGECONVERT_H_
#define _IMAGECONVERT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <setjmp.h>
#include <jpeglib.h>
#include <png.h>

#define my_print(format, ...)  printf("\033[0;31m[%s:%d]%s\033[m\n", __FUNCTION__, __LINE__, format, ##__VA_ARGS__)

typedef struct BITMAPFILEHEADER
{
	unsigned short bfType;		 /*位图文件的类型，必须为BM */
	unsigned int bfSize;		 /*文件大小，以字节为单位*/
	unsigned short bfReserverd1; /*位图文件保留字，必须为0 */
	unsigned short bfReserverd2; /*位图文件保留字，必须为0 */
	unsigned int bfbfOffBits;	 /*位图文件头到数据的偏移量，以字节为单位*/
} BITMAPFILEHEADER;

typedef struct BITMAPINFOHEADER
{
	unsigned int biSize;		   /*该结构大小，字节为单位*/
	unsigned int biWidth;		   /*图形宽度以象素为单位*/
	unsigned int biHeight;		   /*图形高度以象素为单位*/
	unsigned short int biPlanes;   /*目标设备的级别，必须为1 */
	unsigned short int biBitcount; /*颜色深度，每个象素所需要的位数*/
	unsigned int biCompression;    /*位图的压缩类型*/
	unsigned int biSizeImage;	   /*位图的大小，以字节为单位*/
	unsigned int biXPelsPermeter;  /*位图水平分辨率，每米像素数*/
	unsigned int biYPelsPermeter;  /*位图垂直分辨率，每米像素数*/
	unsigned int biClrUsed; 	   /*位图实际使用的颜色表中的颜色数*/
	unsigned int biClrImportant;   /*位图显示过程中重要的颜色数*/
} BITMAPINFOHEADER;

typedef struct SIZEINFO
{
	unsigned int width;
	unsigned int height;
} SIZEINFO;


// 这是扩展的错误处理程序结构
struct my_error_mgr
{
	struct jpeg_error_mgr pub; /* "public" fields */

	jmp_buf setjmp_buffer; /* for return to caller */
};
typedef struct my_error_mgr*  my_error_ptr;

// 以下是将替换标准error_exit方法的例程
METHODDEF(void) my_error_exit(j_common_ptr cinfo)
{
	/*cinfo-> err真的指向一个my_error_mgr结构，所以强制指针*/
	my_error_ptr myerr = (my_error_ptr)cinfo->err;
	/*总是显示消息。*/
  	/*如果我们选择了，我们可以推迟到返回之后。*/
	(*cinfo->err->output_message)(cinfo);
	/*将控制权返回给setjmp点*/
	longjmp(myerr->setjmp_buffer, 1);
}


int NK_BMP2JPG(char* sourcepath, char* destpath);
int NK_BMP2YUV(char* sourcepath, char* destpath);
int NK_BMP2PNG(char* sourcepath, char* destpath);
int NK_JPG2YUV(char* sourcepath, char* destpath);
int NK_JPG2BMP(char* sourcepath, char* destpath);
int NK_JPG2PNG(char* sourcepath, char* destpath);
int NK_YUV2BMP(char* sourcepath, char* destpath, SIZEINFO* size_info);
int NK_YUV2JPG(char* sourcepath, char* destpath, SIZEINFO* size_info);
int NK_YUV2PNG(char* sourcepath, char* destpath, SIZEINFO* size_info);
int NK_PNG2BMP(char* sourcepath, char* destpath);
int NK_PNG2YUV(char* sourcepath, char* destpath);
int NK_PNG2JPG(char* sourcepath, char* destpath);




#endif
