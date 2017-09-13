#include <ncurses.h>
#include <cstring>
#include <string>
#include <fstream>
#include <stdio.h>

using namespace std;

//READ Method, SAVE Method, FILE MENU, SCROLL

const char * readfile(const char *);//reads and converts file to a workable cstring
void savefile();//saves the file to a file name
void inputter(int, int &, char * & temp, WINDOW *,
	      int & x, int & y);//diplays stuff after key strokes. x and y store cursor position
void printpos(int pos, const char * temp, WINDOW *);//prints the file with changes to screen

int lcounter(const char * temp);//returns number of lines of file converted into c string

const char * readfile(const char * filename){
  ifstream myfile(filename);
  string temp;//to be converted to cstring
  string line;//used as a holder for getline
  if(myfile.is_open()){//layer of security to see if file exists
  while(myfile.good()){//keeps looping while not at end or fails
    getline(myfile, line);
    temp = temp + line + "\n";
  }
  myfile.close();
  }
  return temp.c_str();
}

/**
 *@param c is character from window
 *@param pos is line position in file not screen
 *@param x is position in window
 */
void inputter(int c, int & pos, char * & temp, WINDOW * chbox,
	      int & x, int & y, int & flength){
  switch(c){
  case KEY_UP:
    if(x > 0){
      x--;
      wmove(chbox,x,y);
    } 
    if(x == 0 && pos > 0) pos--;
    printpos(pos, temp, chbox);
    break;

  case KEY_DOWN:
    if(x < LINES){
      x++;
      wmove(chbox,x,y);
    }
    if(x == LINES && pos < (flength - LINES)) pos++;
    printpos(pos, temp, chbox);
    break;
    
  case KEY_LEFT:
    if(y > 0){
      y--;
      wmove(chbox,x,y);
    }
  case KEY_RIGHT:
    if(y < COLS && y ){///RIGHT HERE WAS WORKING ON MOVEMENT
      y--;
      wmove(chbox,x,y);
    }
  }//end of switch
}//end of inputter

int main(const int argc, const char * argv[]){
  initscr();
  start_color();  
  noecho();
  cbreak();
 
  WINDOW * textbox;//box to look good
  WINDOW * charbox;//box for characters
  WINDOW * numbox;//box for line numbers
  WINDOW * wrapbox;
  textbox = newwin(LINES - 2, COLS - 2, 1, 1);//dimension and position  
  charbox = newwin(LINES -4, COLS - 9, 2, 6);//Box for actual characters
  numbox = newwin(LINES - 4, 3, 2, 2);
  wrapbox = newwin(LINES - 4, 1, 2, COLS - 3);

  keypad(charbox, true);

  init_pair(1, COLOR_GREEN, COLOR_BLACK);//menu
  init_pair(2, COLOR_CYAN, COLOR_BLACK);//title
  init_pair(3, COLOR_BLUE, COLOR_BLACK);//filename

  wborder(textbox, '|', '|', '=', '=', '3', 'D', '3', 'D');//appearance
 
  attron(COLOR_PAIR(1));
  printw("F1: Menu");
  attroff(COLOR_PAIR(1));
  attron(COLOR_PAIR(2));
  mvprintw(0,(COLS-16)/2,"MLLL 1730 EDITOR");
  attroff(COLOR_PAIR(2));
  attron(COLOR_PAIR(3));
  if(argc == 1)
    mvprintw(LINES - 1, 0,"(New File)");
  else	     
    mvprintw(LINES - 1, 0,argv[1]);
  attroff(COLOR_PAIR(3));
  

  wbkgd(numbox, COLOR_PAIR(1));
  int position = 1;//keeps track of what line is at top of charbox
  string lnum;
  
  for(int i = position; i < position + LINES -4; i++){
    if(i < 10)
      lnum = "  " + to_string(i);
    else if(i < 100)
      lnum = " " + to_string(i);
    else
      lnum = to_string(i);
    wprintw(numbox, lnum.c_str());//i should be changed for line number
    wprintw(wrapbox, "\\");
  }


  wprintw(charbox, readfile(argv[argc - 1]));//EVENTUALLY want to treat wrapped text and such


  refresh();
  wrefresh(textbox);
  wrefresh(numbox);
  wrefresh(wrapbox);
  wrefresh(charbox);
  
  wmove(charbox, 0, 0);//moves cursor to beginning of char box
  bool exit = 0;//is f1 pressed
  char * temp;
  strcpy(temp, readfile(argv[argc - 1]));//copies file to temp
  int x = 0;
  int y = 0;
  int flength = lcounter(temp);
  do{//THIS NEEDs to be console for user input. such as Arrows keys moving cursor, backspace, typing or f1
    int input = wgetch(charbox);    
    inputter(input, position, temp, charbox, x, y, flength);

    /*if(input == '*')//Should be F! for menu
      exit = 1;
    else
    wprintw(charbox,"%c", input);*/ 
    wrefresh(charbox);
  }while(!exit);

  endwin();
  return 0;
}//end of main
