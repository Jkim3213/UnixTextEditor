#include <ncurses.h>
#include <string>
#include <iostream>

#include "Inputter.h"

using namespace std;

int main(const int argc, const char * argv[]){
  initscr();
  start_color();
  noecho();
  cbreak();

  WINDOW * textbox;//box to look good
  WINDOW * charbox;//box for characters
  WINDOW * numbox;//box for line numbers

  textbox = newwin(LINES - 2, COLS - 2, 1, 1);//dimension and position
  charbox = newwin(LINES -4, COLS - 9, 2, 6);//Box for actual characters
  numbox = newwin(LINES - 4, 3, 2, 2);

  keypad(stdscr, true);
  keypad(charbox, true);//f1 key

  init_pair(1, COLOR_GREEN, COLOR_BLACK);//menu
  init_pair(2, COLOR_CYAN, COLOR_BLACK);//title
  init_pair(3, COLOR_BLUE, COLOR_BLACK);//filename
  init_pair(4, COLOR_MAGENTA, COLOR_BLACK);//view mode
  init_pair(5, COLOR_WHITE, COLOR_BLACK);//editing mode
  init_pair(6, COLOR_RED, COLOR_BLACK);//user prompt

  wborder(textbox, '|', '|', '-', '-', '#', '#', '#', '#');//appearance

  wbkgd(numbox, COLOR_PAIR(1));

  attron(COLOR_PAIR(1));
  printw("F1: Menu");
  attroff(COLOR_PAIR(1));
  attron(COLOR_PAIR(2));
  mvprintw(0,(COLS-15)/2,"MLL 1730 EDITOR");
  attroff(COLOR_PAIR(2));
 
  string filename = "";
  
  Inputter in;
  if(argc > 1){
    filename = argv[1];
    in = Inputter(filename);
    //mvprintw(LINES - 1, 0, argv[1]);
  }
  else{
    in = Inputter();
    //mvprintw(LINES - 1, 0,"(New File)");
  }
  in.setBox(charbox, numbox);

  wmove(charbox, 0, 0);

  //  refresh();
  refresh();
  wrefresh(textbox);
  //wrefresh(charbox);

  //int test = 0;
  while(in.getMode() != 'x'){
    //in.setMode('v');//  test purposes
    
    in.updateStatus();
    in.printStatusLine();
    if(in.getMode() == 'v')
      wbkgd(charbox,COLOR_PAIR(4));
    else
      wbkgd(charbox,COLOR_PAIR(5));
    in.printBuff();
    refresh();
    wrefresh(numbox);
    wrefresh(charbox);
    
    int input = wgetch(charbox);
    //test++;
    in.handleInput(input);

  }
  
  refresh();
  delwin(numbox);
  delwin(charbox);
  delwin(textbox);
  delwin(stdscr);
  endwin();
  //std::cout << test;
  return 0;
}
