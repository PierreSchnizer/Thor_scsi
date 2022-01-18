YACC = bison++
LEX = flexc++
YACC = bison
YACCFLAGS= -v --defines #="parser.h" # --yacc
LEX = flex

CC=gcc
CFLAGS=-Wall -pedantic -c
LDLIBS=-lm

MV=mv

.PHONY: all clean clean_sr

all : parser_test # parser.o scanner.o

parser.c : parser.tab.c
	$(MV) -f $< $@

parser.tab.c : parser.y
	$(YACC) $(YACCFLAGS) $<

parser_test : parser_test.o parser.o scanner.o

clean : clean_src
	$(RM) $(RMFLAGS) parser.o scanner.o parser.tab.c parser.c y.tab.c

clean_src :
	$(RM) $(RMFLAGS) parser.tab.c parser.c y.tab.c scanner.c
