YACC = bison++
LEX = flexc++

YACC = bison
YACCFLAGS= -v  --defines -t --yacc #="parser.h" # --yacc
YACC_OUTPUT=y.tab.c
# YACC_OUTPUT=parser.tab.c

LEX = flex -d  --posix-compat


CC=gcc
CFLAGS=-Wall -pedantic -c -DDEBUG
LDLIBS=-lm

MV=mv

.PHONY: all clean clean_src

all : parser_test # parser.o scanner.o

parser.c : $(YACC_OUTPUT)
	$(MV) -f $< $@

$(YACC_OUTPUT) : parser.y
	$(YACC) $(YACCFLAGS) $<

parser_test : parser_test.o parser.o scanner.o

clean : clean_src
	$(RM) $(RMFLAGS) parser.o scanner.o parser_test.o

clean_src :
	$(RM) $(RMFLAGS) parser.tab.c parser.c y.tab.c scanner.c
