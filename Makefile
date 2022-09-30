server: server.c
	gcc server.c -o server -lcrypto

clean:
	rm server -v
