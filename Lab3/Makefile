CC=gcc
CFLAGS=-Wall -pthread
srcs := src/
bins := bin/
name1 := server
name2 := client

all: bin $(bins)$(name1) $(bins)$(name2)

bin:
	mkdir bin

$(bins)%: $(srcs)%.c $(bins)
	$(CC) $< $(CFLAGS) -o $@
clean:
	-rm -f $(bins)$(name1) $(bins)$(name2)
	-rm -rf bin
