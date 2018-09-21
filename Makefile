all: joy2keyng
.PHONY: all

CFLAGS = -ggdb -O0 -lX11

SRC = joy2keyng.c

joy2keyng: $(SRC)
	gcc $(CFLAGS) $(SRC) -o joy2keyng

.PHONY: clean
clean:
	rm -f joy2keyng
