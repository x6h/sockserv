# compiler
cc = gcc
# compiler flags
cflags = -std=gnu99 -Wall -Wextra -pedantic
# object files to compile
objs = sockserv.o helpers.o server.o
# output binary name
bin = sockserv
# installation directory
installdir = /usr/local/bin

all: build

build: $(bin)

$(bin): $(objs)
	$(cc) $(cflags) -o $@ $^

%.o: %.c
	$(cc) $(cflags) -c $^

install: $(bin)
	cp -f $^ $(installdir)

uninstall: $(bin)
	rm -f $(installdir)/$(bin)

clean:
	rm -f $(objs) $(bin)

.PHONY: all build install uninstall clean
