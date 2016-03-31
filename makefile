COMPIL= gcc
VSTD = c99
FLAGS = -Wall -Werror -g

all: main

main: pile.o projet_RO_Pile.c
	$(COMPIL) -std=$(VSTD) $(FLAGS) projet_RO_Pile.c -c
	$(COMPIL) -std=$(VSTD) $(FLAGS) projet_RO_Pile.o pile.o -lglpk -lm -o main

main2: pile.o projet_RO_Pile.c
	$(COMPIL) -std=$(VSTD) $(FLAGS) projet_RO_Pile.c pile.o -o main

pile.o: pile.c 
	$(COMPIL) -std=$(VSTD) $(FLAGS) pile.c -c

doc: cleandoc
	doxygen doxy_config

cleandoc:
	rm -rf doc/
	
clean:
	rm -f *.o