server: src/server.c
	gcc src/server.c src/hashTable.c -o server -I/opt/ssl/include/ -L/opt/ssl/lib/ -lcrypto

clean:
	rm server -v