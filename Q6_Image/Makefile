CC = gcc
## 添加选项-Wall(编译后显示所有警告), -g(令gcc生成调试信息)
CFLAGS = -Wall -g -lpthread

## 获取makefile的工作目录
PWD := $(abspath $(lastword $(MAKEFILE_LIST)))
## 获取当前的目录名(作为可执行文件的名称)
TARGET = $(notdir $(patsubst %/, %, $(dir $(PWD))))

## 源文件为Src下的*.c文件
SOURCE = $(wildcard ./Src/*.c)
## 替换*.c文件为.o
OBJECT = $(patsubst %.c, %.o, $(SOURCE))
## 引用Include下的所有头文件
INCLUDES = -I ./Include

## 将*.o重定位链接成elf文件
$(TARGET) : $(OBJECT)
	$(CC) $^ $(CFLAGS) -o $(TARGET) -L ./lib -lpng -L ./lib -ljpeg

## 将Src下的*.c编译为*.o
%.o : %.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

## 伪目标，规则前加'@'表示不输出打印信息
.PHONY : clean
clean:
	@rm -rf $(OBJECT) $(TARGET)
