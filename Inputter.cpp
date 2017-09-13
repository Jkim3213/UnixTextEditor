#include "Inputter.h"

#include <fstream>
#include <iostream>
#include <unistd.h>

Inputter::Inputter(){//empty constructor
  x = 0;
  y = 0;
  ox = 0;
  oy = 0;
  mode = 'e';
  status = "Editing Mode";
  filename = "(New)";

  buff = new Text();//initializes text REMEMBER TO DEALLOCATE
  buff->appendLine("");//puts empty string so does not break
}//Inputter

Inputter::Inputter(string fn){//constructor with file param
  x = 0;
  y = 0;
  ox = 0;
  oy = 0;
  mode = 'e';
  status = "Editing Mode";
  filename = fn;



  buff = new Text();//initializes text REMEMBER TO DEALLOCATE
  
  ifstream myfile(filename.c_str());
  if(myfile.is_open()){
    while(myfile.good()){
      string temp;
      getline(myfile, temp);
      buff->appendLine(temp);
    }//end of whiel
  }//end of if
  else
    {
      mvwprintw(stdscr, LINES - 1, filename.length() + 1, "(NEW)");
      string emsg = "ERROR:" + (string)strerror(errno);
      attron(COLOR_PAIR(6));
      if(((string)strerror(errno)).compare("Permission denied") == 0)
	mode = 'b';
      mvprintw(LINES - 1, (COLS - emsg.length()) / 2,emsg.c_str());
      //perror("Following error has occured: ");
      attroff(COLOR_PAIR(6));
      buff->appendLine("");
    }
  wmove(chbox, x, y);
}//Inputter

/*
Inputter::~Inputter(){
  delete buff;
}
*/

void Inputter::setBox(WINDOW* charbox, WINDOW* numbox){
  chbox = charbox;
  nbox = numbox;
  getmaxyx(chbox, maxX, maxY);
}

void Inputter::updateStatus(){
  if(fileExists(filename)){
    if(hasWrite(filename))
       mode = 'e';
    else if(hasRead(filename))
      mode = 'v';
    else
      mode = 'b';
  }

  switch(mode)
    {
    case 'e':
      // Editing mode
      status = "Editing Mode";
      break;
    case 'v':
      // View Only mode
      status = "Viewing Mode";
      break;
    case 'b':
      status = "No Permission Mode";
      break;
    case 'x':
      // Exiting
      status = "Exiting";
      break;
    }
  status = status + "  (row:" + to_string(x + 1) + ",col:" + to_string(y + 1) + ")";//includes coordinate of cursor
}

void Inputter::handleInput(int c){
  switch(c)//in viewing and editing, should be able to navigate
    {
    case KEY_LEFT:
      moveLeft();
      return;
    case KEY_RIGHT:
      moveRight();
      return;
    case KEY_UP:
      moveUp();
      return;
    case KEY_DOWN:
      moveDown();
      return;
    case KEY_F(1):
      menu();
      break;
    }
  switch(mode)
    {
    case 'e':
      switch(c)
        {
       
	case KEY_F(1):
	  //	  menu();
	  break;
	  //keys that shouldn't do anything
	case KEY_F(2):
	case KEY_F(3):
	case KEY_F(4):
	case KEY_F(5):
	case KEY_F(6):
	case KEY_F(7):
	case KEY_F(8):
	case KEY_F(9):
	case KEY_F(10):
	case KEY_F(11):
	case KEY_F(12):
	case KEY_DC:
	case KEY_HOME:
	case KEY_END:
	case KEY_NPAGE:
	case KEY_PPAGE: 
	case KEY_IL:
	case KEY_PRINT:
	case 27://escape key
	  break;
        case KEY_BACKSPACE:
	  // The Backspace key
	  if(x > 0 && y == 0)//if at start of a line and below first line
            {
	      y = (short) buff->lines[x-1].length();//change cursor to end of previos line
	      buff->lines[x-1] += buff->lines[x];//previous line = previous line + current line
	      deleteLine();//remove line at x
	      moveUp();
            }
	  else if(!(x == 0 && y == 0))
            {
	      // Removes a character
	      buff->lines[x].erase(--y, 1);//removes character one position behind cursor and moves y back
	    }
	  changed = 1;
	  break;
        case KEY_ENTER:
	case 10:
	  // The Enter key
	  // Bring the rest of the line down
	  if(y < (short)buff->lines[x].length())//if cursor is not at end of line
            {
	      // Put the rest of the line on a new line
	      buff->insertLine(buff->lines[x].substr(y, buff->lines[x].length() - y), x + 1);//insert a line that is the substring from current cursor to end of line at next row
	      // Remove that part of the line
	      buff->lines[x].erase(y, buff->lines[x].length() - y);//remove part of the string from cursor to end of line
            }
	  else
            {
	      buff->insertLine("", x+1);//insert line below
            }
	  y = 0;//change cursor to beginning of line
	  moveDown();
	  changed = 1;
	  break;
        case KEY_BTAB:
        case KEY_CTAB:
        case KEY_STAB:
	case 9:
	  // The Tab key
	  changed = 1;
	  buff->lines[x].insert(y, 4, ' ');//inserts 4 spaces at cursor
	  y += 4;//move cursor
	  break;
        default:
	  // Any other character
	  changed = 1;
	  buff->lines[x].insert(y, 1, char(c));
	  y++;
	  break;
        }
      break;
    }
}//handle input

void Inputter::moveLeft()
{
  if(y > 0)//if cursor is not at beginning of line
    {
      y--;
      wmove(chbox, x - ox, y - oy);
    }
  else if(y == 0 && x > 0){//if cursor is at beginning of line but not at first line
    x--;
    y = (short)buff->lines[x].length();//move to end of previous line
    wmove(chbox, x - ox, y - oy);
  }
}

void Inputter::moveRight()
{
  if(y < (short)buff->lines[x].length())//if not at edge of chbox and not at end of line
    {
      y++;
      wmove(chbox, x - ox, y - oy);
    }
  else if(y == (short)buff->lines[x].length() && x < (short)buff->lines.size() - 1){
    y = 0;
    x++;
    wmove(chbox, x - ox, y - oy);
  }
}

void Inputter::moveUp()
{
  if(x-1 >= 0)
    x--;
  if(y >= (short)buff->lines[x].length())
    y = (short)buff->lines[x].length();//put cursor at end of line
  wmove(chbox, x - ox, y - oy);
}

void Inputter::moveDown()
{
  if(x + 1 < (short)buff->lines.size())//not at end of file
    x++;
  if(y >= (short)buff->lines[x].length())//if not within line, move cursor to end of line
    y = (short)buff->lines[x].length();
  wmove(chbox, x - ox, y - oy);
}

void Inputter::printBuff()
{
  if(x > ox + maxX - 1 || x < ox){//change off set only when at moveing at edge of screen
    if(x - ox < 0){//if moving up at top of screen
      ox = x;
    }
    else{//moving down at bottom
      ox = x - maxX + 1;
    }
  }
  if(y > oy + maxY - 1 || y < oy){//change off set only when at moveing at edge of screen
    if(y - oy < 0){//if moving up at top of screen
      oy = y;
    }
    else{//moving down at bottom
      oy = y - maxY + 1;
    }
  }
  
  for(int i = ox; i < maxX + ox; i++)//refreshes entire screen
    {
      if(i >= (short)buff->lines.size())//if i > file size , clear rest of lines
        {
	  wmove(chbox, i, 0);
	  wclrtoeol(chbox);
	  wmove(nbox, i, 0);
	  wclrtoeol(nbox);
        }
      else
        { if((short)buff->lines[i].length() > oy)//if length reaches the offset
	    mvwprintw(chbox, i - ox, 0, buff->lines[i].substr(oy).c_str());
	  else//if not print empty 
	    mvwprintw(chbox, i - ox, 0, "");
	  wmove(nbox,i -ox, 0);//numbox
	  wclrtoeol(nbox);
	  mvwprintw(nbox, i - ox, 0, to_string(i + 1).c_str());
        }
      wclrtoeol(chbox);
    }
  wmove(chbox, x - ox, y - oy);

}

void Inputter::printStatusLine()
{
  if(changed == 1){
    wmove(stdscr, LINES - 1, 0);
    wclrtoeol(stdscr);
  }
  attron(COLOR_PAIR(3));
  mvwprintw(stdscr, LINES - 1, 0, filename.c_str());//print filename
  attroff(COLOR_PAIR(3));  

  attron(A_REVERSE);
  wmove(stdscr, LINES - 1, COLS - status.length() - 7);//clr status
  clrtoeol();
  mvwprintw(stdscr, LINES-1, COLS - status.length() - 2, status.c_str());//print status
  attroff(A_REVERSE);
  wmove(chbox, x - ox, y - oy);
}

void Inputter::deleteLine()
{
  buff->removeLine(x);
}

void Inputter::deleteLine(int i)
{
  buff->removeLine(i);
}

void Inputter::saveFile(string fn)
{
  if(filename == "(New)")
    {
      // Set filename to untitled
      filename = "untitled";
    }

  ofstream f(fn.c_str());
  string emsg;
  if(f.is_open() && hasWrite(fn))
    {
      for(int i=0; i<(short)buff->lines.size(); i++)
        {
	  f << buff->lines[i] << endl;
        }
      status = "Saved to file!";
      filename = fn;
      mvwprintw(stdscr, LINES - 1, (COLS - status.length()) / 2,status.c_str());
      changed = 0;
   }
  else 
    {
      emsg = "ERROR: No Access: " + fn;

      attron(COLOR_PAIR(6));
      mvwprintw(stdscr, LINES - 1, (COLS - emsg.length()) / 2,emsg.c_str());
      //perror("Following error has occured: ");
      attroff(COLOR_PAIR(6));

    }
  f.close();

 
}

void Inputter::printMid(WINDOW * win, int s_y, int s_x, int w, const char * str, chtype color)
{
  int l, x, y;
  float temp;

  if(win == NULL)
    win = stdscr;
  getyx(win,y,x);
  if(s_x != 0)
    x = s_x;
  if(s_y != 0)
    y = s_y;
  if(w == 0)
    w = 80;

  l = strlen(str);
  temp = (w-l)/2;
  x = s_x + (int)temp;
  wattron(win,color);
  mvwprintw(win, y, x, "%s", str);
  wattroff(win, color);
  refresh();
}

void Inputter::menu()
{
  ITEM ** menu_items;
  int ch;
  MENU * menu;
  WINDOW * menu_win;
  int n_options;

  n_options = sizeof(options)/sizeof(options[0]);
  menu_items = (ITEM **)calloc(n_options, sizeof(ITEM *));
  for(int i=0; i<n_options; i++) {
    menu_items[i] = new_item(options[i],"");
  } // for

  menu = new_menu((ITEM **)menu_items);

  menu_win = newwin(10,40,2,6);
  keypad(menu_win, TRUE);

  set_menu_win(menu, menu_win);
  set_menu_sub(menu, derwin(menu_win,6,38,3,1));

  set_menu_mark(menu, " -> ");

  box(menu_win,0,0);
  wborder(menu_win,'|','|','-','-','#','#','#','#');
  printMid(menu_win,1,0,40,"My Menu",COLOR_PAIR(1));
  mvwaddch(menu_win,2,0,'|');
  mvwhline(menu_win,2,1,'-',38);
  mvwaddch(menu_win,2,39,'|');
  refresh();
  
  post_menu(menu);
  wrefresh(menu_win);

  while((ch = wgetch(menu_win)) != KEY_F(1)) {
    switch(ch) {
    case KEY_DOWN:
      menu_driver(menu, REQ_DOWN_ITEM);
      break;

    case KEY_UP:
      menu_driver(menu, REQ_UP_ITEM);
      break;

    case 10:
      string cur_item = item_name(current_item(menu));
      if(strcmp(cur_item.c_str(), "Open") == 0){
	open();
	goto exit_loop;
      }
      else if(strcmp(cur_item.c_str(), "Save") == 0){
	saveFile(filename);
	goto exit_loop;
      }
      else if(strcmp(cur_item.c_str(), "Save As") == 0){
	saveAs();
	goto exit_loop;
      }
      else if(strcmp(cur_item.c_str(), "Exit") == 0){
	confirmSave();
	mode = 'x';
	goto exit_loop;
      }
      break;
    } // switch
    wrefresh(menu_win);
  } // while
 exit_loop:;

  unpost_menu(menu);
  free_menu(menu);
  for(int i=0; i<n_options; i++)
    free_item(menu_items[i]);
} // menu

void Inputter::confirmSave(){
  string confirm = "Save current progress?(y/n) ";//prompt
  char input;//user input

  if(changed){//if file has changed
    attron(COLOR_PAIR(6));
    mvwprintw(stdscr, LINES - 1, 0, confirm.c_str());
    attroff(COLOR_PAIR(6));
    while(input != 'y' || input != 'n'){
      input = getch();
      if(input == 'y'){
        saveFile(filename);
        break;
      }
      else if(input == 'n')
        break;
      else{
	attron(COLOR_PAIR(6));
        mvwprintw(stdscr, LINES - 1, 0, confirm.c_str());
	attroff(COLOR_PAIR(6));
      }
    }//end of while
  }//end of if
} // confirmSave

void Inputter::open()
{
  string prompt = "Enter filename: ";

  attron(COLOR_PAIR(6));
  mvwprintw(stdscr, LINES - 1, 0, prompt.c_str());
  attroff(COLOR_PAIR(6));

  int cur;//current char
  string ftop;//file to open
  //move(LINES -1, 0);
  //clrtoeol();
  int z = 0;//keeps track of position
  while(cur != 10) {
    cur = getch();
    if(cur == KEY_BACKSPACE){
      if(z > 0){
	z--;
	ftop.erase(ftop.length() - 1, 1);
      }
    }
    else{
      z++;
      ftop += (char)cur;
    }
    move(LINES - 1, prompt.length());
    clrtoeol();
    move(LINES - 1, prompt.length() + z);
    mvprintw(LINES - 1, prompt.length(), ftop.c_str());
  }

  confirmSave();

  ftop.erase(ftop.length() - 1, 1);
  move(LINES -1, 0);
  clrtoeol();
  setInputter(ftop);
} // open

void Inputter::setInputter(string fn){
  x = 0;
  y = 0;
  ox = 0;
  oy = 0;
  changed = 0;
  mode = 'e';
  status = "Editing Mode";
  filename = fn;

  buff = new Text();//initializes text REMEMBER TO DEALLOCATE

  ifstream myfile(filename.c_str());
  if(myfile.is_open()){
    while(myfile.good()){
      string temp;
      getline(myfile, temp);
      buff->appendLine(temp);
    }//end of whiel
  }//end of if
  else
    {
      mvwprintw(stdscr, LINES - 1, filename.length() + 1, "(NEW)");
      string emsg = "ERROR:" + (string)strerror(errno);
      attron(COLOR_PAIR(6));
      if(((string)strerror(errno)).compare("Permission denied") == 0)
        mode = 'b';
      mvprintw(LINES - 1, (COLS - emsg.length()) / 2,emsg.c_str());
      //perror("Following error has occured: ");
      attroff(COLOR_PAIR(6));
      buff->appendLine("");
    }
} //setInputter

bool Inputter::fileExists(string fn) {
  ifstream infile(fn.c_str());
  return infile.good();
} // fileExists

void Inputter::saveAs() {
  string prompt = "Enter filename: ";

  attron(COLOR_PAIR(6));
  mvwprintw(stdscr, LINES - 1, 0, prompt.c_str());
  attroff(COLOR_PAIR(6));

  int cur;//current char
  string ftop;//file to open
  //move(LINES -1, 0);
  //clrtoeol();
  /*  
while(cur != 10) {
    cur = getch();
    ftop += cur;
    mvprintw(LINES - 1, prompt.length(), ftop.c_str());
  }

  ftop.erase(ftop.length() - 1, 1);
  move(LINES -1, 0);
  clrtoeol();
  */
  int z = 0;  

  while(cur != 10) {
    cur = getch();
    if(cur == KEY_BACKSPACE){
      if(z > 0){
        z--;
        ftop.erase(ftop.length() - 1, 1);
      }
    }
    else{
      z++;
      ftop += (char)cur;
    }
    move(LINES - 1, prompt.length());
    clrtoeol();
    move(LINES - 1, prompt.length() + z);
    mvprintw(LINES - 1, prompt.length(), ftop.c_str());
  }
  ftop.erase(ftop.length() - 1, 1);

  string overwrite = "Overwrite existing file?(y/n) ";//prompt
  char input;//user input
  string emsg = "Cannot overwrite existing file";

  if(fileExists(ftop)){
    attron(COLOR_PAIR(6));
    mvwprintw(stdscr, LINES - 1, 0, overwrite.c_str());
    // attroff(COLOR_PAIR(6));
    // move(LINES -1, 0);
    clrtoeol();
    while(input != 'y' || input != 'n'){
      input = getch();
      if(input == 'y'){
	//	filename = ftop;
        saveFile(ftop);
        break;
      }
      else if(input == 'n')
        break;
      else{
	attron(COLOR_PAIR(6));
        mvwprintw(stdscr, LINES - 1, 0, overwrite.c_str());
	attroff(COLOR_PAIR(6));
      }
    }//end of while
  }//end of if 
  else{//file does not exist 
    //filename = ftop;
    saveFile(ftop);
  }
  move(LINES -1, 0);
  printw("                             ");


} // saveAs

bool Inputter::hasRead(string fn){//return true if file exists and has read permission
  
  return (access(fn.c_str(),R_OK) == 0);
}

bool Inputter::hasWrite(string fn){//return true if file exists and has write permission
  
  return (access(fn.c_str(),W_OK) == 0);

  /*
  struct stat sb;
  stat(fn.c_str(), &sb);
  if(fileExists(fn) && (bool)(sb.st_mode & S_IWUSR))
    return true;
  else
    return false;
  */
}
