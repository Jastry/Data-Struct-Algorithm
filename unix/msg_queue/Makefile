.PHONY:all
all:client server
client:client.c comm.c
	gcc -g -o $@ $^
server:server.c comm.c
	gcc -g -o $@ $^
.PHONY:clean
clean:
	rm -f client server

