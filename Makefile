SRC_DIRS = ./src

CC = gcc
OPT = -O0
CFLAGS = 
LIBS = -I/opt/ssl/include/ -L/opt/ssl/lib/ -lcrypto


server: ${SRC_DIRS}/%.c
	${CC} ${OPT} ${CFLAGS} -o $@ $^ ${LIBS}

clean:
	rm server -v