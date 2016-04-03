COMPIL= gcc
VSTD = c99
FLAGS = -Wall -Werror -g

all: main

main: pile.o BinPacking_CARAT-VUYLSTEKE.c
	$(COMPIL) -std=$(VSTD) $(FLAGS) BinPacking_CARAT-VUYLSTEKE.c -c
	$(COMPIL) -std=$(VSTD) $(FLAGS) BinPacking_CARAT-VUYLSTEKE.o pile.o -lglpk -lm -o main

main2: pile.o BinPacking_CARAT-VUYLSTEKE.c
	$(COMPIL) -std=$(VSTD) $(FLAGS) BinPacking_CARAT-VUYLSTEKE.c pile.o -o main

pile.o: pile.c 
	$(COMPIL) -std=$(VSTD) $(FLAGS) pile.c -c

doc: cleandoc
	doxygen doxy_config

cleandoc:
	rm -rf doc/
	
clean:
	rm -f *.o