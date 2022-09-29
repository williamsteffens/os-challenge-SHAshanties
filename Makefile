server: server.c
	gcc server.c -o server -lcrypto -w

serverw: server.c
	gcc server.c -o server -lcrypto

clean:
	rm server -v
