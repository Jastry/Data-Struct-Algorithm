ROOT_PATH=$(shell pwd)
MYSQL_INCLUDE=$(ROOT_PATH)/mysql_lib/include
MYSQL_LIB=$(ROOT_PATH)/mysql_lib/lib

#LDFLAGS=-mysqlclient

.PHONY:all
all:test

test:test.o mysql_signals.o 
	g++ -o $@ $^ -L$(MYSQL_LIB) -lmysqlclient -lpthread
%.o:%.cpp
	g++ -c $< -I$(MYSQL_INCLUDE)

.PHONY:clean
clean:
	rm -f *.o test

.PHONY:debug
	echo $(LD_LIBRARY_PATH)
