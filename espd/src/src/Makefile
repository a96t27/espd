BUILDDIR = ../build/espd
BINDIR = ../bin
CFLAGS += -Wall -Wextra -pedantic 
CPPFLAGS +=  -I../include
LDFLAGS += -lblobmsg_json -lubox -lubus -lserialport -lcjson
BIN = $(BINDIR)/espd
SRCS = $(wildcard *.c)
OBJS = $(SRCS:%.c=../build/espd/%.o)

.PHONY: clean all objects build run

all: $(BIN)

run: all
	sudo $(BIN)

$(BIN): $(OBJS)
	mkdir -p $(BUILDDIR)
	mkdir -p $(BINDIR)
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)

build: $(BIN)

objects: $(OBJS)

$(BUILDDIR)/%.o: %.c
	mkdir -p $(BUILDDIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -c -o $@ $^

clean:
	${RM} -rf $(BIN) $(BUILDDIR)
