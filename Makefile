








server: src/main.c
	gcc src/main.c -o server -I/opt/ssl/include/ -L/opt/ssl/lib/ -lcrypto

clean:
	rm server -v