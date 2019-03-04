
CFLAGS=-Wall -g

all: variants

variants: variants.c
	$(CC) -o variants $^

test: testc testpy

testc: variants
	./test.sh ./variants

testpy:
	./test.sh python variants.py

clean:
	rm -f *.o variants test.out test.exp

.PHONY: test testc testpy clean

