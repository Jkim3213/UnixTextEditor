1730ed: Main.o Text.o Inputter.o
	g++ -o 1730ed Main.o Text.o Inputter.o -lncurses -lmenu 

Main.o: Main.cpp
	g++ -Wall --std=c++11 -g -O0 -pedantic-errors -c Main.cpp -lncurses 

Text.o: Text.cpp Text.h
	g++ -Wall --std=c++11 -g -O0 -pedantic-errors -c Text.cpp -lncurses 

Inputter.o: Inputter.cpp Inputter.h
	g++ -Wall --std=c++11 -g -O0 -pedantic-errors -c Inputter.cpp -lncurses -lmenu

clean:
	rm -f 1730ed
	rm -f *.o
