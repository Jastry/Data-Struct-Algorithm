ROOTPATH=$(shell pwd)
MYSQL_INCLUDE=$(ROOTPATH)/mysql_lib/include
MYSQL_LIB=$(ROOTPATH)/mysql_lib/lib
export LD_LIBRARY_PATH=$(MYSQL_LIB)

.PHONY:all

all:test	
test:test.o mysql_api.o 
	@g++ -o $@ $^ -L$(MYSQL_LIB) -lmysqlclient 
%.o:%.cpp
	@g++ -c $< -I$(MYSQL_INCLUDE)

.PHONY:clean
clean:
	@rm -f *.o test
.PHONY:debug
debug:
	#echo $(MYSQL_LIB)
	#echo $(MYSQL_INCLUDE)
	echo $(LD_LIBRARY_PATH)
#shell 命令前加入@符号则不在屏幕上打印该命令
