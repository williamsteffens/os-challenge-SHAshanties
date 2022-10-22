CC = gcc
OPT = -O0
CFLAGS = 
LIBS = -I /opt/ssl/include/ -L /opt/ssl/lib/ -lcrypto -pthread

SDIR = src
ODIR = obj

SRCS := $(shell find src/ -name '*.c')
_OBJS := $(SRCS:.c=.o)
OBJS := $(patsubst %,$(ODIR)/%,$(_OBJS))
DEPS := $(shell find src -name '*.h')

TARGET = server



$(ODIR)/$(SDIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) $(OPT) -c -o $@ $< $(CFLAGS) -g

$(TARGET): $(OBJS)
	$(CC) $(OPT) $(CFLAGS) -g -o $@ $^ $(LIBS)

clean:
	rm server -v