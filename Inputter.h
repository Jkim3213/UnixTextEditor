#ifndef INPUTTER_H
#define INPUTTER_H

#include <ncurses.h>
#include <menu.h>
#include <string.h>

#include "Text.h"

using namespace std;

class Inputter{
 private:
  int x, y;// row, col for cursor
  char mode;// viewing, editing, closing
  Text* buff;// array of buffer
  string status, filename;// status describes type of mode
  WINDOW* chbox, * nbox;
  int maxX, maxY; //max rows and cols od chbox
  int ox, oy;//offset x, y in the file
  int oox, ooy;//old offsets of x and y
  bool changed = 0;//if the file is modified

  void printMid(WINDOW * win, int s_y, int s_x, int w, const char * str, chtype color);
  void menu();// Creates a menu
  const char * options[4] = {"Open", "Save", "Save As", "Exit"};// array pf menu items
  void subMenu(WINDOW * win);// Creates a window that holds user prompt
  void open(); // Function for the "Open" key in menu
  void confirmSave();//asks user if saved
  void saveAs();// Function for the "Save As" key in menu
  bool fileExists(string fn);// Checks if file exist

  bool hasRead(string fn);
  bool hasWrite(string fn);
  
  // Cursor movement
  void moveUp();
  void moveDown();
  void moveLeft();
  void moveRight();

  void deleteLine();// Deletes current line
  void deleteLine(int);// Deletes line <int>

  void saveFile(string fn);// Saves buffer into the file
  void setInputter(string);


 public:
  Inputter();// Normal constructor
  Inputter(string);// Constructor accepting filename
  //  ~Inputter();

  void setBox(WINDOW* charbox, WINDOW* numbox);

  char getMode() {return mode;}//returns the mode
  
  //  void setMode(char m) {mode = m;}// exists for test purposes only

  void handleInput(int);// Handles keyboard input
  void printBuff();//prints entire buffer to screen 
  void printStatusLine();// Prints the status line (like vim!!!)
  void updateStatus();// Updates the status line (text, not display)
};

#endif
