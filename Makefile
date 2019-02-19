
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
test: variants
	@echo "Expanded Variants" > $(TESTOUT)
	@echo "Expanded Variants" > $(TESTEXP)
	./variants "foo|bar" "ba[r|z]" "qu[u]x" >> $(TESTOUT)
	@echo "foo;bar;bar;baz;quux;qux" | tr ";" "\n" >> $(TESTEXP)
	./variants "foo|ba[r|z[aar]]|qu[u]x" >> $(TESTOUT)
	@echo "foo;bar;bazaar;baz;quux;qux" | tr ";" "\n" >> $(TESTEXP)
	./variants "It[[em|alic]iz|erat]e[d], please" >> $(TESTOUT)
	@echo "Itemized, please" >> $(TESTEXP)
	@echo "Itemize, please" >> $(TESTEXP)
	@echo "Italicized, please" >> $(TESTEXP)
	@echo "Italicize, please" >> $(TESTEXP)
	@echo "Iterated, please" >> $(TESTEXP)
	@echo "Iterate, please" >> $(TESTEXP)
	./variants "a[1|2|3]b[4|5]c" >> $(TESTOUT)
	@echo "a1b4c;a1b5c;a2b4c;a2b5c;a3b4c;a3b5c" | tr ";" "\n" >> $(TESTEXP)
	./variants "J[oh[ann]] W[olfgang] [v[on] ]Goethe" >> $(TESTOUT)
	@echo "Johann Wolfgang von Goethe" >> $(TESTEXP)
	@echo "Johann Wolfgang v Goethe" >> $(TESTEXP)
	@echo "Johann Wolfgang Goethe" >> $(TESTEXP)
	@echo "Johann W von Goethe" >> $(TESTEXP)
	@echo "Johann W v Goethe" >> $(TESTEXP)
	@echo "Johann W Goethe" >> $(TESTEXP)
	@echo "Joh Wolfgang von Goethe" >> $(TESTEXP)
	@echo "Joh Wolfgang v Goethe" >> $(TESTEXP)
	@echo "Joh Wolfgang Goethe" >> $(TESTEXP)
	@echo "Joh W von Goethe" >> $(TESTEXP)
	@echo "Joh W v Goethe" >> $(TESTEXP)
	@echo "Joh W Goethe" >> $(TESTEXP)
	@echo "J Wolfgang von Goethe" >> $(TESTEXP)
	@echo "J Wolfgang v Goethe" >> $(TESTEXP)
	@echo "J Wolfgang Goethe" >> $(TESTEXP)
	@echo "J W von Goethe" >> $(TESTEXP)
	@echo "J W v Goethe" >> $(TESTEXP)
	@echo "J W Goethe" >> $(TESTEXP)
	# Testing weird and invalid syntax:
	./variants "[[[foo]]]" >> $(TESTOUT)
	@echo "foo" >> $(TESTEXP)
	./variants "[|||]" >> $(TESTOUT)   # no output
	./variants "f]o|o]" "[f[o[o" >> $(TESTOUT)
	@echo "fo;o;foo;fo;f" | tr ";" "\n" >> $(TESTEXP)
	./variants "x[]x" -- "xx[" -- "x[|]x" -- "x[[]]x" >> $(TESTOUT)
	@echo "xx;xx;--;xx;xx;--;xx;xx;--;xx;xx;xx" | tr ";" "\n" >> $(TESTEXP)
	@diff $(TESTOUT) $(TESTEXP) && echo "TEST PASSED" || echo "TEST FAILED"

