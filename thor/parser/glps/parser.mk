YACC = bison++
LEX = flexc++
YACC = bison
YACCFLAGS= -v --defines="parser.h" # --yacc
LEX = flex

CC=gcc
CFLAGS=-Wall -pedantic
MV=mv


all : parser.o scanner.o

parser.c : parser.tab.c
	$(MV) -f $< $@

parser.tab.c : parser.y
	$(YACC) $(YACCFLAGS) $<


clean :
	$(RM) $(RMFLAGS) parser.o scanner.o parser.tab.c parser.c y.tab.c
