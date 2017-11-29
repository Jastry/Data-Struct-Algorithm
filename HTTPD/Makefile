ROOT_PATH=$(shell pwd)
CONF=$(ROOT_PATH)/config
LOG=$(ROOT_PATH)/log
PLUGIN=$(ROOT_PATH)/plugin
WWWROOT=$(ROOT_PATH)/wwwroot

FLAGS=-D_STDOUT_#Makefile 定义一个宏，需要 -D 选项，后面紧跟宏定义的名称
LDFLAGS=-lpthread
cc=gcc
bin=httpd
src=httpd.c main.c
obj=$(shell echo $(src) | sed 's/\.c/\.o/g')#注意这里 echo (src) 时需要用 shell

$(bin):$(obj)
	@$(cc) -o $@ $^ $(LDFLAGS)

#$(obj):$(src)
%.o:%.c
	@$(cc) -c $< $(FLAGS)
.PHONY:clean
clean:
	@rm -f *.o $(bin)
.PHONY:debug
debug:
	echo $(src)
	echo $(obj)
	echo $(bin)
