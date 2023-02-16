CC = gcc
CFLAGS = -Wall -g  # All Warning Messages ComingUp and Into GDB Mode

PWD := $(abspath $(lastword $(MAKEFILE_LIST)))
TARGET = $(notdir $(patsubst %/,%,$(dir $(PWD)))) #$(notdir $(PWD))
#TARGET = main      # Finally Executable File Generated


# replace the file end with .c to the suffix .o
SOURCE = $(wildcard ./src/*.c)

# OBJECT = main.o add.o max.o
OBJECT = $(patsubst %.c, %.o, $(SOURCE))

INCLUDES = -I ./inc

$(TARGET) : $(OBJECT)
	$(CC) $^ $(CFLAGS) -o $(TARGET)

%.o : %.c
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@ 

.PHONY : clean
clean:
	@rm -rf $(OBJECT) $(TARGET)
