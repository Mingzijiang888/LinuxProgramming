**题目6：实现BMP、YUV、PNG、JPEG图片的互相转换。参考：BMP、YUV的转换需要自己实现算法，PNG、JPEG可使用开源库libjpeg-turbo、libpng。**

具体实现：

1、换了新电脑，重新make编译通过不了了，报如下错误：

./lib/libpng.so: undefined reference to `pow@GLIBC_2.29'
./lib/libpng.so: undefined reference to `inflateValidate@ZLIB_1.2.9'

可能涉及到底层库的安装问题，稍后有时间再解决