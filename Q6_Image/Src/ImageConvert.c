#include "ImageConvert.h"
 
/**
 * @brief 获得bmp图像的头信息
 * 
 * @param fin 输入文件流
 * @param file_Header bmp文件头部
 * @param info_Header bmp信息头部
 * @return int S:0  F:-1
 */
static int NK_BMP_GetHeaderInfo(FILE *fin, BITMAPFILEHEADER* file_Header, BITMAPINFOHEADER* info_Header)
{
	int ret = 0;
	fseek(fin, 0, SEEK_SET);
	ret = fread(&(file_Header->bfType), 2, 1, fin);
	if (ret != 1)
	{
		my_print("failed to read file header");
		return -1;
	}
	if (file_Header->bfType != 0x4D42)
	{
		my_print("Not bmp file.");
		return -1;
	}

	fseek(fin, 2, SEEK_SET);
	fread(&file_Header->bfSize, 4, 1, fin);
	fseek(fin, 6, SEEK_SET);
	fread(&file_Header->bfReserverd1, 2, 1, fin);
	fseek(fin, 8, SEEK_SET);
	fread(&file_Header->bfReserverd2, 2, 1, fin);
	fseek(fin, 10, SEEK_SET);
	fread(&file_Header->bfbfOffBits, 4, 1, fin);
	fseek(fin, 14, SEEK_SET);
	fread(&info_Header->biSize, 4, 1, fin);
	fseek(fin, 18, SEEK_SET);
	fread(&info_Header->biWidth, 4, 1, fin);
	fseek(fin, 22, SEEK_SET);
	fread(&info_Header->biHeight, 4, 1, fin);
	fseek(fin, 26, SEEK_SET);
	fread(&info_Header->biPlanes, 2, 1, fin);
	fseek(fin, 28, SEEK_SET);
	fread(&info_Header->biBitcount, 2, 1, fin);
	fseek(fin, 30, SEEK_SET);
	fread(&info_Header->biCompression, 4, 1, fin);
	fseek(fin, 34, SEEK_SET);
	fread(&info_Header->biSizeImage, 4, 1, fin);
	fseek(fin, 38, SEEK_SET);
	fread(&info_Header->biXPelsPermeter, 4, 1, fin);
	fseek(fin, 42, SEEK_SET);
	fread(&info_Header->biYPelsPermeter, 4, 1, fin);
	fseek(fin, 46, SEEK_SET);
	fread(&info_Header->biClrUsed, 4, 1, fin);
	fseek(fin, 50, SEEK_SET);
	fread(&info_Header->biClrImportant, 4, 1, fin);
	fseek(fin, 54, SEEK_SET);
	return 0;
}

/**
 * @brief 设置bmp图像的头信息
 * 
 * @param fin 输入文件流
 * @param file_Header bmp文件头部
 * @param info_Header bmp信息头部
 */
static void NK_BMP_SetHeaderInfo(const FILE *fin, BITMAPFILEHEADER* file_Header, BITMAPINFOHEADER* info_Header)
{
	file_Header->bfType = 19778;
	file_Header->bfReserverd1 = 0;
	file_Header->bfReserverd2 = 0;
	file_Header->bfbfOffBits = 54;

	info_Header->biSize = 40;
	info_Header->biPlanes = 1;
	info_Header->biBitcount = 24;
	// info_Header->biCompression = 0;
	// info_Header->biXPelsPermeter = 0;
	// info_Header->biYPelsPermeter = 0;
	// info_Header->biClrImportant = 0;
	// info_Header->biClrImportant = 0;
	info_Header->biSizeImage = info_Header->biWidth * info_Header->biHeight\
										* (info_Header->biBitcount / 8);
	file_Header->bfSize = info_Header->biSizeImage + 54;
}

/**
 * @brief 将RGB缓存中的数据转化为BGR形式
 * 
 * @param rgbBuf rgb缓存区
 * @param size_info 图像尺寸信息
 */
static void NK_BMP_RGB2BGR(unsigned char* rgbBuf, SIZEINFO* size_info)
{
	int i = 0, j = 0;
	int width = size_info->width;
	int height = size_info->height;
	unsigned char temp = 0;
	
	// 将RGB编码方式转换成BGR编码方式
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			temp = rgbBuf[width * i * 3 + j * 3];
			 rgbBuf[width * i * 3 + j * 3] =  rgbBuf[width * i * 3 + j * 3 + 2];
			 rgbBuf[width * i * 3 + j * 3 + 2] = temp;
		}
	}

	// unsigned char* pBuf = (unsigned char*)malloc(sizeof(unsigned char) * width * 3);
	// for (i = 0, j = height - 1; i < j; i++, j--)
	// {
	// 	memcpy(pBuf, rgbBuf + i * width * 3, width * 3);
	// 	memcpy(rgbBuf + i * width * 3, rgbBuf + j * width * 3, width * 3);
	// 	memcpy(rgbBuf + j * width * 3, pBuf, width * 3);
	// }
	// free(pBuf);
	// pBuf = NULL;
}

/**
 * @brief 获得bmp图像的rgb数据并缓存
 * 
 * @param fin 输入文件流
 * @param rgbBuf rgb缓存区
 * @param size_info 图像尺寸信息
 * @return int S:0  F:-1
 */
static int NK_BMP_GetRGB(FILE* fin, unsigned char** rgbBuf, const SIZEINFO* size_info)
{
	int ret = 0;
	int i = 0, j = 0;
	if (rgbBuf == NULL)
	{
		my_print("buffer not exists.");
		return -1;
	}
	int width = size_info->width;
	int height = size_info->height;
	
	*rgbBuf = (unsigned char*)malloc(sizeof(unsigned char) * width * height * 3);
	if (*rgbBuf == NULL)
	{
		my_print("malloc failed.");
		return -1;
	}
	memset(*rgbBuf, 0, sizeof(unsigned char) * width * height * 3);

	fseek(fin, 54, SEEK_SET);
	ret = fread(*rgbBuf, 1, width * height * 3, fin);
	if (ret != width * height * 3)
	{
		my_print("read bmp failed");
		return -1;
	}

	unsigned char temp = 0;

	// 将BGR编码方式转换成RGB编码方式
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < height; j++)
		{
			temp = (*rgbBuf)[width * i * 3 + j * 3];
			 (*rgbBuf)[width * i * 3 + j * 3] =  (*rgbBuf)[width * i * 3 + j * 3 + 2];
			 (*rgbBuf)[width * i * 3 + j * 3 + 2] = temp;
		}
	}

	unsigned char* pBuf = (unsigned char*)malloc(sizeof(unsigned char) * width * 3);
	for (i = 0, j = height - 1; i < j; i++, j--)
	{
		memcpy(pBuf, (*rgbBuf) + i * width * 3, width * 3);
		memcpy((*rgbBuf) + i * width * 3, (*rgbBuf) + j * width * 3, width * 3);
		memcpy((*rgbBuf) + j * width * 3, pBuf, width * 3);
	}

	free(pBuf);
	pBuf = NULL;

	return 0;	
}

/**
 * @brief 获得jpg图像的rgb数据并缓存
 * 
 * @param fin 输入文件流
 * @param rgbBuf rgb缓存区
 * @param size_info 图像尺寸信息
 * @return int S:0  F:-1
 */
static int NK_JPG_GetRGB(FILE* fin, unsigned char** rgbBuf, SIZEINFO* size_info)
{
	if (rgbBuf == NULL)
	{
		my_print("buffer not exists.");
		return -1;
	}
	// 定义一个JPEG解码信息结构体
    struct jpeg_decompress_struct cinfo;
	// 定义一个错误信息结构体
    struct my_error_mgr jerr;
	memset(&cinfo, 0, sizeof(struct jpeg_decompress_struct));
	memset(&jerr, 0, sizeof(struct my_error_mgr));
	JSAMPARRAY buffer = NULL;	// unsigned char*
	int row_stride = 0;

	/* 开始初始化错误对象*/
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	if (setjmp(jerr.setjmp_buffer))
	{
		jpeg_destroy_decompress(&cinfo);
		my_print("setjmp error.");
		return -1;
	}

	/*初始化解码对象了*/
	jpeg_create_decompress(&cinfo);
	/*把源文件与解码对象关联起来 */
	jpeg_stdio_src(&cinfo, fin);
	/*读取JPEG图片的头数据*/
	(void) jpeg_read_header(&cinfo, TRUE);
	/*开始解码*/
	(void) jpeg_start_decompress(&cinfo);
	
	// 得到一行数据所占用的字节数
	row_stride = cinfo.output_width * cinfo.output_components;
	/*根据行字节数去分配堆空间*/
	buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);
	
	unsigned char* ptr = NULL;
	int x = 0, y = 0;
	unsigned char r = 0, g = 0, b = 0;
	int width = cinfo.output_width;
	int height = cinfo.output_height;
	size_info->height = height;
	size_info->width = width;

	*rgbBuf = (unsigned char*)malloc(width * height * 3);
	if (rgbBuf == NULL)
	{
		jpeg_destroy_decompress(&cinfo);
		my_print("malloc failed.");
		return -1;
	}
	memset(*rgbBuf, 0, sizeof(unsigned char) * width * height * 3);

	//解码就在这里开始，上面都是准备工作
	while (cinfo.output_scanline < cinfo.output_height)
	{
		(void) jpeg_read_scanlines(&cinfo, buffer, 1);

		ptr = buffer[0];
		y = height - cinfo.output_scanline;		// bmp图是从下往上
		
		for (x = 0; x < width; x++)
		{
			r = *(ptr++);
			g = *(ptr++);
			b = *(ptr++);

			(*rgbBuf)[width * y * 3 + x * 3] = r;
			(*rgbBuf)[width * y * 3 + x * 3 + 1] = g;
			(*rgbBuf)[width * y * 3 + x * 3 + 2] = b;
		}
	}

	//结束解码对象
    (void) jpeg_finish_decompress(&cinfo);

    //销毁解码对象
    jpeg_destroy_decompress(&cinfo);

	return 0;
}

/**
 * @brief 获得yuv图像的rgb数据并缓存
 * 
 * @param fin 输入文件流
 * @param rgbBuf rgb缓存区
 * @param size_info 图像尺寸信息
 * @return int S:0  F:-1
 */
static int NK_YUV_GetRGB(FILE* fin, unsigned char** rgbBuf, const SIZEINFO* size_info)
{
	int i = 0, j = 0, k = 0;
	int width = size_info->width;
	int height = size_info->height;

	int RowDataSize   = ((width * 24 + 31) / 32) * 4;	/*每行的数据总字节数*/
	int RowBlankSize  = RowDataSize % 3;				/*每行空出（不足R,G,B三通道-视为废弃点）无需采样的字节数*/
	int RowReadSize   = RowDataSize - RowBlankSize;		/*每行需要采样的字节数*/
	int TotalReadSize = RowReadSize * height;		    /*按4:2:0采样所需总字节数*/

	int NumOfWrite = RowDataSize * height;
	*rgbBuf = (unsigned char*)malloc(NumOfWrite);
	if((*rgbBuf) == NULL)
	{
		my_print("malloc failed.");
		return -1;
	}
	memset(*rgbBuf, 0, sizeof(NumOfWrite));
	unsigned char* pWBuf = *rgbBuf;

	unsigned char* yBuf = (unsigned char*)malloc(TotalReadSize / 3);
	unsigned char* uBuf = (unsigned char*)malloc(TotalReadSize / 12);		// 4:2:0采样后数据
	unsigned char* vBuf = (unsigned char*)malloc(TotalReadSize / 12);
	if (yBuf == NULL || uBuf == NULL || vBuf == NULL)
	{
		my_print("malloc failed");
		return -1;
	}
	memset(yBuf, 0, sizeof(char) * TotalReadSize / 3);
	memset(uBuf, 0, sizeof(char) * TotalReadSize / 12);
	memset(vBuf, 0, sizeof(char) * TotalReadSize / 12);
	
	fread(yBuf, 1, width*height, fin);
	fseek(fin, width*height, SEEK_SET);
	fread(uBuf, 1, width*height / 4, fin);
	fseek(fin, 5 * width*height / 4, SEEK_SET);
	fread(vBuf, 1, width*height / 4, fin);
	fseek(fin, 0, SEEK_SET);

	for (i = 0; i < width*height; i++)
	{
		if(yBuf[i] == 1)
		{
			yBuf[i] = 0;
		}
	}
	for (i = 0; i < width*height / 4; i++)
	{
		if(uBuf[i] == 1)
		{
			uBuf[i] = 0;
		}
		if(vBuf[i] == 1)
		{
			vBuf[i] = 0;
		}
	}

	// yuv 转为 rgb
	for (i = height - 1; i >= 0; i--)
	{
		for(j = 0; j < width; j++)
		{
			*(*rgbBuf) =	yBuf[i*width + j + 2]
						+ 1.770 * (uBuf[(i/2)*width/2 + j/2] - 128);
			(*rgbBuf)++;
			*(*rgbBuf) =	yBuf[i*width + j]
						- 0.714 * (vBuf[(i/2)*width/2 + j/2] - 128)
						- 0.343 * (uBuf[(i/2)*width/2 + j/2] - 128);
			(*rgbBuf)++;
			*(*rgbBuf) =	yBuf[i*width + j + 1]
						+ 1.403 * (vBuf[(i/2)*width/2 + j/2] - 128);
			(*rgbBuf)++;
		}
		for(k = 0; k < RowBlankSize; k++)
		{
			*(*rgbBuf) = 0;
			(*rgbBuf)++;
		}
	}

	(*rgbBuf) = pWBuf;
	for (i = 0; i < NumOfWrite; i++)
	{
		(*rgbBuf)[i] = ((*rgbBuf)[i] < 0 ? 0 : (*rgbBuf)[i]);
		(*rgbBuf)[i] = ((*rgbBuf)[i] > 255 ? 255 : (*rgbBuf)[i]);
	}

	free(yBuf);
	yBuf = NULL;
	free(vBuf);
	vBuf = NULL;
	free(uBuf);
	uBuf = NULL;
	pWBuf = NULL;

	return 0;
}

/**
 * @brief 获得png图像的rgb数据并缓存
 * 
 * @param fin 输入文件流
 * @param rgbBuf rgb缓存区
 * @param size_info 图像尺寸信息
 * @return int S:0  F:-1
 */
static int NK_PNG_GetRGB(FILE* fin, unsigned char** rgbBuf, SIZEINFO* size_info)
{
	int PNG_BYTES_TO_CHECK = 4;
	char buf[PNG_BYTES_TO_CHECK];

	if (fread(buf, 1, PNG_BYTES_TO_CHECK, fin) != PNG_BYTES_TO_CHECK)
	{
		my_print("Not png file.");
		return -1;
	}

	png_uint_32 width = 0, height = 0;
	png_structp png_ptr = NULL;		//libpng的结构体ptr
	png_infop info_ptr = NULL;		//libpng的信息ptr

	// 创建一个png_structp结构体，如果不想自定义错误处理，后3个参数可以传NULL
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL)
	{
		my_print("read png error.");
		return -1;
	}

	// 创建一个info_ptr，这是必须的
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		my_print("create png info error.");
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return -1;
	}

	// 如果上面png_create_read_struct没有自定义错误处理，这里是必须的
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return -1;
	}

	// 初始化文件IO
	rewind(fin);
	png_init_io(png_ptr, fin);

	// 读取png图片信息
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_STRIP_ALPHA, NULL);

	if (png_ptr != NULL && info_ptr != NULL)
	{
		// 获取图像的宽高
		width = png_get_image_width(png_ptr, info_ptr);
		height= png_get_image_height(png_ptr, info_ptr);
	}
	size_info->width = width;
	size_info->height = height;

	// 获取图像所有的像素数据
	png_bytep* row_pointers;	// 指针数组，数组中的每一个指针都指向一行图像数据
	row_pointers = png_get_rows(png_ptr, info_ptr);

	int i = 0;   // j = 0;
	*rgbBuf = (unsigned char*)malloc(sizeof(unsigned char) * width * height * 3);
	if ((*rgbBuf) == NULL)
	{
		my_print("malloc failed.");
		return -1;
	}
	memset(*rgbBuf, 0, sizeof(unsigned char) * width * height * 3);

	unsigned char* pBuf = *rgbBuf;
	for (i = 0; i < height; i++)
	{
		// more convenient
		memcpy(*rgbBuf, row_pointers[i], width * 3);
		*rgbBuf = *rgbBuf + width * 3;
	}
	*rgbBuf = pBuf;
	pBuf = NULL;

	png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);

	return 0; 
}

/**
 * @brief 由rgb缓存数据转为bmp图像格式并保存文件
 * 
 * @param destpath 目标路径
 * @param file_Header bmp文件头部
 * @param info_Header bmp信息头部
 * @param rgbBuf rgb缓存区
 * @return int S:0  F:-1
 */
static int NK_RGB2BMP(const char* destpath, const BITMAPFILEHEADER* file_Header, const BITMAPINFOHEADER* info_Header, const unsigned char* rgbBuf)
{
	int ret = 0;
	int fout = open(destpath, O_WRONLY | O_TRUNC | O_CREAT, 0777);
	if (fout <= 0)
	{
		my_print("path error");
		return -1;
	}
	int width =  info_Header->biWidth;
	int height = info_Header->biHeight;

	write(fout, &(file_Header->bfType), 2);
	write(fout, &(file_Header->bfSize), 4);
	write(fout, &(file_Header->bfReserverd1), 2);
	write(fout, &(file_Header->bfReserverd2), 2);
	write(fout, &(file_Header->bfbfOffBits), 4);
	write(fout, info_Header, 40);
	ret = write(fout, rgbBuf, width * height * 3);
	if (ret != width * height * 3)
	{
		my_print("write error.");
	}

	ret = close(fout);
	if (ret == -1)
	{
		my_print("close failed."); 
		return -1;
	}
	return 0;
}

/**
 * @brief 由rgb缓存数据转为jpg图像格式并保存文件
 * 
 * @param destpath 目标路径
 * @param rgbBuf rgb缓存区
 * @param size_info 图像尺寸信息
 * @return int S:0  F:-1
 */
static int NK_RGB2JPG(const char* destpath, unsigned char* rgbBuf, const SIZEINFO* size_info)
{
	int ret = 0;
	FILE* fout = NULL;
	if ((fout = fopen(destpath, "w+")) == NULL)
	{
		my_print("path error.");
		return -1;
	}

	// 不能初始化该两结构体，会导致Empty JPEG，无法写入
	struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);

    jpeg_create_compress(&cinfo);
	JSAMPROW row_pointer[1];
	// int row_stride = size_info->width * 3;	
	jpeg_stdio_dest(&cinfo, fout);
	cinfo.image_width = size_info->width;
	cinfo.image_height = size_info->height;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;

	/*压缩并写入rgb信息*/
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, 90, TRUE);
	jpeg_start_compress(&cinfo, TRUE);

	int width = size_info->width;
	int height = size_info->height;
	// while (cinfo.next_scanline < cinfo.image_height)
	// {
	// 	row_pointer[0] = & rgbBuf[cinfo.next_scanline * row_stride];
	// 	(void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
	// }
	for (int i = 0; i < height; i++)
	{
		row_pointer[0] = rgbBuf + i * width * 3;
	 	(void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

	
	ret = fclose(fout);
	if (ret == -1)
	{
		my_print("close failed.");
		return -1;
	}
	fout = NULL;

	return 0;
}

/**
 * @brief 由rgb缓存数据转为yuv图像格式并保存文件
 * 
 * @param destpath 目标路径
 * @param rgbBuf rgb缓存区
 * @param size_info 图像尺寸信息
 * @return int S:0  F:-1
 */
static int NK_RGB2YUV(const char* destpath, unsigned char* rgbBuf, const SIZEINFO* size_info)
{
	FILE *fout = fopen(destpath, "w+");
	if (fout == NULL)
	{
		my_print("open failed.");
		return -1;
	}

	int ret = 0;
	int i = 0, j = 0;
	int width = size_info->width;
	int height = size_info->height;

	int RowDataSize   = ((width * 24 + 31) / 32) * 4;
	int RowBlankSize  = RowDataSize % 3;
	int RowReadSize   = RowDataSize - RowBlankSize;
	int TotalReadSize = RowReadSize * height;

	// 分配缓存空间
	unsigned char* yBuf   = (unsigned char*)malloc(TotalReadSize / 3);
	unsigned char* yuv_u  = (unsigned char*)malloc(TotalReadSize / 3);		// 采样前全部数据
	unsigned char* yuv_v  = (unsigned char*)malloc(TotalReadSize / 3);
	unsigned char* uBuf   = (unsigned char*)malloc(TotalReadSize / 12);		// 4:2:0采样后数据
	unsigned char* vBuf   = (unsigned char*)malloc(TotalReadSize / 12);
	if (yBuf == NULL || yuv_u == NULL || yuv_v == NULL || uBuf == NULL || vBuf == NULL)
	{
		my_print("malloc failed");
		return -1;
	}
	memset(yBuf,   0, sizeof(char) * TotalReadSize / 3);
	memset(yuv_u,  0, sizeof(char) * TotalReadSize / 3);
	memset(yuv_v,  0, sizeof(char) * TotalReadSize / 3);
	memset(uBuf,   0, sizeof(char) * TotalReadSize / 12);
	memset(vBuf,   0, sizeof(char) * TotalReadSize / 12);

	// unsigned char *pBuf = rgbBuf;	// 指向rgbBuf初始地址
	// int RowOffset = 0;

	// // 垂直方向倒序提取(转正序)
	// for(int j = height - 1; j >= 0; j--)
	// {
	// 	rgbBuf = pBuf + RowReadSize * RowOffset;
	// 	RowOffset++;
	// 	fseek(fout, RowDataSize * j, SEEK_SET);
	// 	fread(rgbBuf, 1, RowReadSize, fin);
	// }
	// rgbBuf = pBuf;
	// pBuf = NULL;

	// 将 rgb数据 转化成 yuv数据
	for(i = 0; i < TotalReadSize; i += 3)
	{
		yBuf[i/3] = 0.114 * rgbBuf[i + 2] + 0.587 * rgbBuf[i + 1] + 0.299 * rgbBuf[i];
	}
	for(i = 0; i < TotalReadSize / 3; i++)
	{
		yuv_u[i] = 0.5 * rgbBuf[3*i + 2] - 0.3316 * rgbBuf[3*i + 1] - 0.1684 * rgbBuf[3*i] + 128;
	}
	for(i = 0; i < TotalReadSize / 3; i++)
	{
		yuv_v[i] = -0.081 * rgbBuf[3*i + 2] - 0.419 * rgbBuf[3*i + 1] + 0.5 * rgbBuf[3*i] + 128;
	}

	// 对u，v分量分别采样
	for(i = 0; i < height; i = i + 2)
	{
		for(j = 0; j < width; j = j + 2)
		{
			uBuf[(i/2)*width/2 + j/2] = (
				yuv_u[i*width + j]+
				yuv_u[i*width + j+1]+
				yuv_u[(i+1)*width + j]+
				yuv_u[(i+1)*width + j+1]) / 4;
		}
	}
	free(yuv_u);
	yuv_u = NULL;
	for(int i = 0; i < height; i = i + 2)
	{
		for(int j = 0; j < width; j = j + 2)
		{
			vBuf[(i/2)*width/2 + j/2] = (
				yuv_v[i*width + j] +
				yuv_v[i*width + j+1] +
				yuv_v[(i+1)*width + j] +
				yuv_v[(i+1)*width + j+1]) / 4;
		}
	}
	free(yuv_v);
	yuv_v = NULL;

	// 亮电平信号量化后码电平分配：在对分量信号进行8比特均匀量化时，共分为256个等间隔的量化级。
	// 为了防止信号变动造成过载，在256级上端留16级，下端留16级作为信号超越动态范围的保护带。
	// 色差信号量化后码电平分配：色差信号经过归一化处理后，动态范围为-0.5-0.5，让色差零电平对应码电平128，色差信号总共占225个量化级。
	// 在256级上端留16级，下端留16级作为信号超越动态范围的保护带。
	for(i = 0; i < TotalReadSize / 3; i++)
	{
		yBuf[i] = (yBuf[i] < 16  ? 16  : yBuf[i]);
		yBuf[i] = (yBuf[i] > 240 ? 240 : yBuf[i]);
	}
	for(i = 0; i < TotalReadSize / 12; i++)
	{
		uBuf[i] = (uBuf[i] < 16  ? 16  : uBuf[i]);
		uBuf[i] = (uBuf[i] > 240 ? 240 : uBuf[i]);
		vBuf[i] = (vBuf[i] < 16  ? 16  : vBuf[i]);
		vBuf[i] = (vBuf[i] > 240 ? 240 : vBuf[i]);
	}

	// 转换写入yuv图像文件
	int TotalWriteSize = TotalReadSize / 2;		// yuv图像数据总字节数
	unsigned char* WriteBuf = (unsigned char*)malloc(TotalWriteSize);
	if (WriteBuf == NULL)
	{
		printf("WriteBuf creat failed\n");
		return -1;
	}
	memset(WriteBuf, 0, sizeof(TotalWriteSize));
	unsigned char *pWBuf = WriteBuf;		// 指向WriteBuf初始地址
	memcpy(WriteBuf, yBuf, TotalReadSize / 3);
	WriteBuf = pWBuf + TotalReadSize / 3;
	memcpy(WriteBuf, uBuf, TotalReadSize / 12);
	WriteBuf = pWBuf + 5 * (TotalReadSize / 12);
	memcpy(WriteBuf, vBuf, TotalReadSize / 12);
	WriteBuf = pWBuf;
	
	free(yBuf);
	yBuf = NULL;
	free(vBuf);
	vBuf = NULL;
	free(uBuf);
	uBuf = NULL;
	pWBuf = NULL;

	ret = fwrite(WriteBuf, 1, sizeof(char) * TotalReadSize / 2, fout);
	if (ret != sizeof(char) * TotalReadSize / 2)
	{
		my_print("write error.");
	}

	free(WriteBuf);
	WriteBuf = NULL;

	ret = fclose(fout);
	if (ret != 0)
	{
		my_print("close failed.");
		return -1;
	}
	fout = NULL;

	return 0;
}

/**
 * @brief 由rgb缓存数据转为png图像格式并保存文件
 * 
 * @param destpath 目标路径
 * @param rgbBuf rgb缓存区
 * @param size_info 图像尺寸信息
 * @return int S:0  F:-1
 */
static int NK_RGB2PNG(const char* destpath, unsigned char* rgbBuf, const SIZEINFO* size_info)
{
	int ret = 0;
	FILE* fout = NULL;
	if ((fout = fopen(destpath, "w+")) == NULL)
	{
		my_print("path error.");
		return -1;
	}

	int width = size_info->width;
	int height = size_info->height;
	png_structp png_ptr = NULL;		//libpng的结构体ptr
	png_infop info_ptr = NULL;		//libpng的信息ptr

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info_ptr = png_create_info_struct(png_ptr);
	if(!png_ptr && !info_ptr)
	{
		my_print("create stuct failed.");
		fclose(fout);
		return -1;
	}
	png_set_check_for_invalid_index(png_ptr, 0);

	// 设置错误跳转
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		fclose(fout);
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		return -1;
	}

	// 设置输出控制（标准C流）
	png_init_io(png_ptr, fout);
	// 设置图片属性
	png_set_IHDR(png_ptr, info_ptr, width, height, 8,
                    PNG_COLOR_TYPE_RGB,
                    PNG_INTERLACE_NONE,
                    PNG_COMPRESSION_TYPE_BASE,
                    PNG_FILTER_TYPE_BASE);

	png_set_packing(png_ptr);
	// 写文件头
	png_write_info(png_ptr, info_ptr);

	if (height > PNG_UINT_32_MAX / sizeof(png_bytep))
	{
		my_print("image too large.");
		return -1;
	}


	// 定义行指针，并将rgb数据写入行指针
	png_bytep* row_pointer = png_malloc(png_ptr, height *(sizeof(png_bytep)));
	
	int i = 0;
	if (row_pointer == NULL)
	{
		my_print("malloc failed.");
		fclose(fout);
		return -1;
	}
	memset(row_pointer, 0, height *(sizeof(png_bytep)));

	for (i = 0; i < height; i++)
	{
		row_pointer[i] = png_malloc(png_ptr, width * 3);
		if (row_pointer[i] == NULL)
		{
			my_print("malloc failed.");
			fclose(fout);
			return -1;
		}
		memset(row_pointer[i], 0, width * 3);
	}

	// png_bytep* pR = row_pointer;
	for (i = 0; i < height; i++)
	{
		memcpy(row_pointer[i], rgbBuf, width * 3);
		rgbBuf = rgbBuf + width * 3;
	}

	// 往文件写入转换好的rgb数据
	png_write_image(png_ptr, row_pointer);

	// 写入文件结束符号
	png_write_end(png_ptr, info_ptr);

	for (i = 0; i < height; i++)
	{
		png_free(png_ptr, row_pointer[i]);
		row_pointer[i] = NULL;
	}
	png_free(png_ptr, row_pointer);

	png_destroy_write_struct(&png_ptr, &info_ptr);

	ret = fclose(fout);
	if (ret == -1)
	{
		my_print("close failed.");
		return -1;
	}
	fout = NULL;

	return 0;
}



int NK_BMP2JPG(char* sourcepath, char* destpath)
{
	int ret = 0;
	if (sourcepath == NULL || destpath == NULL)
	{
		my_print("path error.");
	}
	FILE* fin = fopen(sourcepath, "rb");
	if(fin == NULL)
	{
		my_print("open failed.");
		return -1;
	}

	unsigned char* rgbBuf = NULL;
	SIZEINFO size_info;
	memset(&size_info, 0, sizeof(struct SIZEINFO));
	BITMAPFILEHEADER file_Header;
	BITMAPINFOHEADER info_Header;
	memset(&file_Header, 0, sizeof(struct BITMAPFILEHEADER));
	memset(&info_Header, 0, sizeof(struct BITMAPINFOHEADER));

	ret = NK_BMP_GetHeaderInfo(fin, &file_Header, &info_Header);
	if (ret == -1)
	{
		return -1;
	}
	size_info.width = info_Header.biWidth;
	size_info.height = info_Header.biHeight;

	ret = NK_BMP_GetRGB(fin, &rgbBuf, &size_info);
	if (ret == -1)
	{
		return -1;
	}

	ret = NK_RGB2JPG(destpath, rgbBuf, &size_info);
	if (ret == -1)
	{
		return -1;
	}

	free(rgbBuf);
	rgbBuf = NULL;

	ret = fclose(fin);
	if (ret == -1)
	{
		my_print("close failed");
		return -1;
	}

	return 0;
}

int NK_BMP2YUV(char* sourcepath, char* destpath)
{
	int ret = 0;
	if (sourcepath == NULL || destpath == NULL)
	{
		my_print("path error.");
	}
	FILE* fin = fopen(sourcepath, "rb");
	if(fin == NULL)
	{
		my_print("open failed.");
		return -1;
	}

	unsigned char* rgbBuf = NULL;
	SIZEINFO size_info;
	memset(&size_info, 0, sizeof(struct SIZEINFO));
	BITMAPFILEHEADER file_Header;
	BITMAPINFOHEADER info_Header;
	memset(&file_Header, 0, sizeof(struct BITMAPFILEHEADER));
	memset(&info_Header, 0, sizeof(struct BITMAPINFOHEADER));

	ret = NK_BMP_GetHeaderInfo(fin, &file_Header, &info_Header);
	if (ret == -1)
	{
		return -1;
	}
	size_info.width = info_Header.biWidth;
	size_info.height = info_Header.biHeight;

	ret = NK_BMP_GetRGB(fin, &rgbBuf, &size_info);
	if (ret == -1)
	{
		return -1;
	}

	ret = NK_RGB2YUV(destpath, rgbBuf, &size_info);
	if (ret == -1)
	{
		return -1;
	}

	free(rgbBuf);
	rgbBuf = NULL;

	ret = fclose(fin);
	if (ret == -1)
	{
		my_print("close failed");
		return -1;
	}
	fin = NULL;

	return 0;
}

int NK_BMP2PNG(char* sourcepath, char* destpath)
{
	int ret = 0;
	if (sourcepath == NULL || destpath == NULL)
	{
		my_print("path error.");
	}
	FILE* fin = fopen(sourcepath, "rb");
	if(fin == NULL)
	{
		my_print("open failed.");
		return -1;
	}

	unsigned char* rgbBuf = NULL;
	SIZEINFO size_info;
	memset(&size_info, 0, sizeof(struct SIZEINFO));
	BITMAPFILEHEADER file_Header;
	BITMAPINFOHEADER info_Header;
	memset(&file_Header, 0, sizeof(struct BITMAPFILEHEADER));
	memset(&info_Header, 0, sizeof(struct BITMAPINFOHEADER));

	ret = NK_BMP_GetHeaderInfo(fin, &file_Header, &info_Header);
	if (ret == -1)
	{
		return -1;
	}
	size_info.width = info_Header.biWidth;
	size_info.height = info_Header.biHeight;

	ret = NK_BMP_GetRGB(fin, &rgbBuf, &size_info);
	if (ret == -1)
	{
		return -1;
	}

	ret = NK_RGB2PNG(destpath, rgbBuf, &size_info);
	if (ret == -1)
	{
		return -1;
	}

	free(rgbBuf);
	rgbBuf = NULL;

	ret = fclose(fin);
	if (ret == -1)
	{
		my_print("close failed");
		return -1;
	}

	return 0;
}

int NK_JPG2YUV(char* sourcepath, char* destpath)
{
	int ret = 0;
	if (sourcepath == NULL || destpath == NULL)
	{
		my_print("path error.");
		return -1;
	}
	FILE* fin = fopen(sourcepath, "rb");
	if(fin == NULL)
	{
		my_print("open failed.");
		return -1;
	}

	unsigned char* rgbBuf = NULL;
	SIZEINFO size_info;
	memset(&size_info, 0, sizeof(struct SIZEINFO));

	ret = NK_JPG_GetRGB(fin, &rgbBuf, &size_info);
	if (ret == -1)
	{
		return -1;
	}

	ret = NK_RGB2YUV(destpath ,rgbBuf, &size_info);
	if (ret == -1)
	{
		return -1;
	}

	// malloc in NK_JPG_GetRGB()
	free(rgbBuf);
	rgbBuf = NULL;

	ret = fclose(fin);
	if (ret == -1)
	{
		my_print("close failed");
		return -1;
	}

	return 0;
}

int NK_JPG2BMP(char* sourcepath, char* destpath)
{
	int ret = 0;
	if (sourcepath == NULL || destpath == NULL)
	{
		my_print("path error.");
		return -1;
	}
	FILE* fin = fopen(sourcepath, "rb");
	if(fin == NULL)
	{
		my_print("open failed.");
		return -1;
	}

	unsigned char* rgbBuf = NULL;
	SIZEINFO size_info;
	memset(&size_info, 0, sizeof(struct SIZEINFO));
	BITMAPFILEHEADER file_Header;
	BITMAPINFOHEADER info_Header;
	memset(&file_Header, 0, sizeof(struct BITMAPFILEHEADER));
	memset(&info_Header, 0, sizeof(struct BITMAPINFOHEADER));

	ret = NK_JPG_GetRGB(fin, &rgbBuf, &size_info);
	if (ret == -1)
	{
		return -1;
	}
	info_Header.biWidth = size_info.width;
	info_Header.biHeight = size_info.height;

	NK_BMP_RGB2BGR(rgbBuf, &size_info);
	
	NK_BMP_SetHeaderInfo(fin, &file_Header, &info_Header);

	ret = NK_RGB2BMP(destpath ,&file_Header, &info_Header, rgbBuf);
	if (ret == -1)
	{
		return -1;
	}

	// malloc in NK_JPG_GetRGB()
	free(rgbBuf);
	rgbBuf = NULL;

	ret = fclose(fin);
	if (ret == -1)
	{
		my_print("close failed");
		return -1;
	}

	return 0;
}

int NK_JPG2PNG(char* sourcepath, char* destpath)
{
	int ret = 0;
	if (sourcepath == NULL || destpath == NULL)
	{
		my_print("path error.");
		return -1;
	}
	FILE* fin = fopen(sourcepath, "rb");
	if(fin == NULL)
	{
		my_print("open failed.");
		return -1;
	}

	unsigned char* rgbBuf = NULL;
	SIZEINFO size_info;
	memset(&size_info, 0, sizeof(struct SIZEINFO));

	ret = NK_JPG_GetRGB(fin, &rgbBuf, &size_info);
	if (ret == -1)
	{
		return -1;
	}

	ret = NK_RGB2PNG(destpath, rgbBuf, &size_info);
	if (ret == -1)
	{
		return -1;
	}

	// malloc in NK_JPG_GetRGB()
	free(rgbBuf);
	rgbBuf = NULL;

	ret = fclose(fin);
	if (ret == -1)
	{
		my_print("close failed");
		return -1;
	}

	return 0;
}

int NK_YUV2BMP(char* sourcepath, char* destpath, SIZEINFO* size_info)
{
	int ret = 0;
	if (sourcepath == NULL || destpath == NULL)
	{
		my_print("path error.");
		return -1;
	}
	FILE* fin = fopen(sourcepath, "rb");
	if(fin == NULL)
	{
		my_print("open failed.");
		return -1;
	}

	unsigned char* rgbBuf = NULL;
	BITMAPFILEHEADER file_Header;
	BITMAPINFOHEADER info_Header;
	memset(&file_Header, 0, sizeof(struct BITMAPFILEHEADER));
	memset(&info_Header, 0, sizeof(struct BITMAPINFOHEADER));

	info_Header.biWidth = size_info->width;
	info_Header.biHeight = size_info->height;
	NK_BMP_SetHeaderInfo(fin, &file_Header, &info_Header);

	ret = NK_YUV_GetRGB(fin ,&rgbBuf, size_info);
	if (ret == -1)
	{
		return -1;
	}

	// NK_BMP_RGB2BGR(rgbBuf, size_info);

	ret = NK_RGB2BMP(destpath ,&file_Header, &info_Header, rgbBuf);
	if (ret == -1)
	{
		return -1;
	}

	free(rgbBuf);
	rgbBuf = NULL;

	ret = fclose(fin);
	if (ret == -1)
	{
		my_print("close failed");
		return -1;
	}

	return 0;
}

int NK_YUV2JPG(char* sourcepath, char* destpath, SIZEINFO* size_info)
{
	int ret = 0;
	if (sourcepath == NULL || destpath == NULL)
	{
		my_print("path error.");
		return -1;
	}
	FILE* fin = fopen(sourcepath, "rb");
	if(fin == NULL)
	{
		my_print("open failed.");
		return -1;
	}

	unsigned char* rgbBuf = NULL;

	ret = NK_YUV_GetRGB(fin ,&rgbBuf, size_info);
	if (ret == -1)
	{
		return -1;
	}

	NK_BMP_RGB2BGR(rgbBuf, size_info);

	ret = NK_RGB2JPG(destpath ,rgbBuf, size_info);
	if (ret == -1)
	{
		return -1;
	}

	free(rgbBuf);
	rgbBuf = NULL;

	ret = fclose(fin);
	if (ret == -1)
	{
		my_print("close failed");
		return -1;
	}

	return 0;
}

int NK_YUV2PNG(char* sourcepath, char* destpath, SIZEINFO* size_info)
{
	int ret = 0;
	if (sourcepath == NULL || destpath == NULL)
	{
		my_print("path error.");
		return -1;
	}
	FILE* fin = fopen(sourcepath, "rb");
	if(fin == NULL)
	{
		my_print("open failed.");
		return -1;
	}

	unsigned char* rgbBuf = NULL;

	ret = NK_YUV_GetRGB(fin ,&rgbBuf, size_info);
	if (ret == -1)
	{
		return -1;
	}

	NK_BMP_RGB2BGR(rgbBuf, size_info);

	ret = NK_RGB2PNG(destpath ,rgbBuf, size_info);
	if (ret == -1)
	{
		return -1;
	}

	free(rgbBuf);
	rgbBuf = NULL;

	ret = fclose(fin);
	if (ret == -1)
	{
		my_print("close failed");
		return -1;
	}

	return 0;
}

int NK_PNG2BMP(char* sourcepath, char* destpath)
{
	int ret = 0;
	if (sourcepath == NULL || destpath == NULL)
	{
		my_print("path error.");
		return -1;
	}
	FILE* fin = fopen(sourcepath, "rb");
	if(fin == NULL)
	{
		my_print("open failed.");
		return -1;
	}

	unsigned char* rgbBuf = NULL;
	SIZEINFO size_info;
	memset(&size_info, 0, sizeof(struct SIZEINFO));
	BITMAPFILEHEADER file_Header;
	BITMAPINFOHEADER info_Header;
	memset(&file_Header, 0, sizeof(struct BITMAPFILEHEADER));
	memset(&info_Header, 0, sizeof(struct BITMAPINFOHEADER));

	ret = NK_PNG_GetRGB(fin, &rgbBuf, &size_info);
	if (ret == -1)
	{
		return -1;
	}
	info_Header.biWidth = size_info.width;
	info_Header.biHeight = size_info.height;

	NK_BMP_RGB2BGR(rgbBuf, &size_info);
	
	NK_BMP_SetHeaderInfo(fin, &file_Header, &info_Header);

	ret = NK_RGB2BMP(destpath ,&file_Header, &info_Header, rgbBuf);
	if (ret == -1)
	{
		return -1;
	}

	free(rgbBuf);
	rgbBuf = NULL;

	ret = fclose(fin);
	if (ret == -1)
	{
		my_print("close failed");
		return -1;
	}
	fin = NULL;

	return 0;
}

int NK_PNG2YUV(char* sourcepath, char* destpath)
{
	int ret = 0;
	if (sourcepath == NULL || destpath == NULL)
	{
		my_print("path error.");
		return -1;
	}
	FILE* fin = fopen(sourcepath, "rb");
	if(fin == NULL)
	{
		my_print("open failed.");
		return -1;
	}

	unsigned char* rgbBuf = NULL;
	SIZEINFO size_info;
	memset(&size_info, 0, sizeof(struct SIZEINFO));

	ret = NK_PNG_GetRGB(fin, &rgbBuf, &size_info);
	if (ret == -1)
	{
		return -1;
	}

	ret = NK_RGB2YUV(destpath ,rgbBuf, &size_info);
	if (ret == -1)
	{
		return -1;
	}

	free(rgbBuf);
	rgbBuf = NULL;

	ret = fclose(fin);
	if (ret == -1)
	{
		my_print("close failed");
		return -1;
	}
	fin = NULL;

	return 0;
}

int NK_PNG2JPG(char* sourcepath, char* destpath)
{
	int ret = 0;
	if (sourcepath == NULL || destpath == NULL)
	{
		my_print("path error.");
		return -1;
	}
	FILE* fin = fopen(sourcepath, "rb");
	if(fin == NULL)
	{
		my_print("open failed.");
		return -1;
	}

	SIZEINFO size_info;
	memset(&size_info, 0, sizeof(struct SIZEINFO));
	unsigned char* rgbBuf = NULL;

	ret = NK_PNG_GetRGB(fin, &rgbBuf, &size_info);
	if (ret == -1)
	{
		return -1;
	}

	ret = NK_RGB2JPG(destpath ,rgbBuf, &size_info);
	if (ret == -1)
	{
		return -1;
	}

	free(rgbBuf);
	rgbBuf = NULL;

	ret = fclose(fin);
	if (ret == -1)
	{
		my_print("close failed");
		return -1;
	}
	fin = NULL;

	return 0;
}



