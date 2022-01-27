YACC = bison++
LEX = flexc++

YACC = bison
YACCFLAGS= -v  --defines -t --yacc #="parser.h" # --yacc
YACC_OUTPUT=y.tab
# YACC_OUTPUT=parser.tab
YACC_OUTPUT_STAMP=$(YACC_OUTPUT).stamp

YACC_OUTPUT_SRC=$(YACC_OUTPUT).c
YACC_OUTPUT_HEADER=$(YACC_OUTPUT).h
TOUCH=touch
LEX = flexc++ -d  --posix-compat


CC=gcc
CFLAGS=-Wall -pedantic -c -DDEBUG
LDLIBS=-lm

MV=mv

.PHONY: all clean clean_src

all : parser_test # parser.o scanner.o


$(YACC_OUTPUT_SRC) : parser.y
	$(YACC) $(YACCFLAGS) $<

$(YACC_OUTPUT_HEADER) : $(YACC_OUTPUT_SRC)

parser.c : $(YACC_OUTPUT_SRC)
	$(MV) -f $< $@

parser.h : $(YACC_OUTPUT_HEADER)
	$(MV) -f $< $@

parser.o : parser.c parser.h

parser_test : parser_test.o parser.o scanner.o

clean : clean_src
	$(RM) $(RMFLAGS) parser.o scanner.o parser_test.o

clean_src :
	$(RM) $(RMFLAGS) $(YACC_OUTPUT_SRC) $(YACC_OUTPUT_HEADER) $(YACC_OUTPUT_STAMP) scanner.c

clean_defaults :
	$(RM) $(RMFLAGS) parser.tab.c y.tab.c  y.tab.h parser.tab.h
