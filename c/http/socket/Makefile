.PHONY:all
all:client server
client:client.c
	gcc -g -o  $@ $^ 
server:server.c
	gcc -g -o  $@ $^ -pthread
.PHONY:clean
clean:
	rm -f client server
