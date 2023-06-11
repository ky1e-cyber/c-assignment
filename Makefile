
EXE = main

IN = $(wildcard tests/*-input.txt)
ACT = $(IN:-input.txt=-actual.txt)
PASS = $(IN:-input.txt=.passed)


.PHONY: all clean test

all: test

clean:
	@rm -f $(PASS)
	rm -f $(ACT) $(EXE)


HEADERS = $(wildcard ./*.h)

$(EXE): *.c $(HEADERS)
	$(CC) *.c -o $@


test: $(PASS)
	@echo "All tests passed"

$(PASS): %.passed: %-input.txt %-expected.txt $(EXE)
	@echo "Running test $*..."
	@rm -f $@
	./$(EXE) $*-input.txt $*-actual.txt
	diff $*-expected.txt $*-actual.txt
	@touch $@