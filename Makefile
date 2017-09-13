1730ed: Main.o Text.o Inputter.o
	g++ -lncurses -lmenu -o 1730ed Main.o Text.o Inputter.o

Main.o: Main.cpp
	g++ -Wall -g -O0 -pedantic-errors -lncurses -c Main.cpp

Text.o: Text.cpp Text.h
	g++ -Wall -g -O0 -pedantic-errors -lncurses -c Text.cpp

Inputter.o: Inputter.cpp Inputter.h
	g++ -Wall -g -O0 -pedantic-errors -lncurses -lmenu -c Inputter.cpp

clean:
	rm -f 1730ed
	rm -f *.o
