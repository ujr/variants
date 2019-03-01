
CFLAGS=-Wall -g

TESTOUT=test.out   # actual output
TESTEXP=test.exp   # expected output

all: variants

variants: variants.c
	$(CC) -o variants $^

clean:
	rm -f *.o variants $(TESTOUT) $(TESTEXP)

.PHONY: test clean

# Super simple automated testing: append actual expansion
# results and expected results to two files, then compare
# those files
test: testc testpy

testc: variants
	./test.sh ./variants
testpy:
	./test.sh python variants.py
