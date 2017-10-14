ROOT_PATH=$(shell pwd)
CONF=ROOT_PATH/config
LOG=ROOT_PATH/log
PLUGIN=ROOT_PATH/plugin
WWWROOT=ROOT_PATH/wwwroot

FLAGS=#
LDFLAGS=#
cc=gcc
bin=httpd
src=httpd.c main.c
obj=$(shell echo $(src) | sed 's/\.c/\.o/g')#????

$(bin):$(src)
	$(cc) -o $@ $^ $(LDFLAGS)

#$(obj):$(src)
%.o:%.c
	$(cc) -c $<
.PHONY:clean
clean:
	rm -f *.o $(bin)
.PHONY:debug
debug:
	echo $(src)
	echo $(obj)
	echo $(bin)
